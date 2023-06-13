// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbcImporter.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif

THIRD_PARTY_INCLUDES_START
#include <Alembic/AbcCoreAbstract/TimeSampling.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
THIRD_PARTY_INCLUDES_END

#include "Animation/AnimSequence.h"
#include "AnimationUtils.h"
#include "Async/ParallelFor.h"
#include "ComponentReregisterContext.h"
#include "Doodle/Abc/AbcFile.h"
#include "Doodle/Abc/AbcImportUtilities.h"
#include "Doodle/Abc/DoodleAbcAssetImportData.h"
#include "Doodle/Abc/EigenHelper.h"
#include "Editor.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "GeometryCacheCodecV1.h"
// #include "E:/UnrealEngine/Engine/Source/Runtime/Engine/Public/MaterialShared.h"
// #include "E:/UnrealEngine/Engine/Source/Runtime/Engine/Public/MaterialShared.h"
// #include "MaterialShared.h" // IWYU pragma: keep
//  sb一样的编译系统,在 5.2版本会出现找不到这个类的问题,  卧槽, 必须这么去补上
// UENUM()
// enum EMaterialDomain {
//  /** The material's attributes describe a 3d surface. */
//  MD_Surface UMETA(DisplayName = "Surface"),
//  /** The material's attributes describe a deferred decal, and will be mapped onto the decal's frustum. */
//  MD_DeferredDecal UMETA(DisplayName = "Deferred Decal"),
//  /** The material's attributes describe a light's distribution. */
//  MD_LightFunction UMETA(DisplayName = "Light Function"),
//  /** The material's attributes describe a 3d volume. */
//  MD_Volume UMETA(DisplayName = "Volume"),
//  /** The material will be used in a custom post process pass. */
//  MD_PostProcess UMETA(DisplayName = "Post Process"),
//  /** The material will be used for UMG or Slate UI */
//  MD_UI UMETA(DisplayName = "User Interface"),
//  /** The material will be used for runtime virtual texture (Deprecated). */
//  MD_RuntimeVirtualTexture UMETA(Hidden),
//
//  MD_MAX
//};

#include "Materials/Material.h"
#include "MeshUtilities.h"
#include "Misc/FeedbackContext.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Modules/ModuleManager.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "RawIndexBuffer.h"
#include "Rendering/SkeletalMeshModel.h"
#include "StaticMeshAttributes.h"
#include "StaticMeshOperations.h"
#include "Stats/StatsMisc.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "Utils.h"

#define LOCTEXT_NAMESPACE "AbcImporter"

DEFINE_LOG_CATEGORY_STATIC(LogAbcImporter, Verbose, All);

#define OBJECT_TYPE_SWITCH(a, b, c)                      \
  if (AbcImporterUtilities::IsType<a>(ObjectMetaData)) { \
    a TypedObject = a(b, Alembic::Abc::kWrapExisting);   \
    ParseAbcObject<a>(TypedObject, c);                   \
    bHandled = true;                                     \
  }

#define PRINT_UNIQUE_VERTICES 0

FAbcImporter::FAbcImporter() : ImportSettings(nullptr), AbcFile(nullptr) {}

FAbcImporter::~FAbcImporter() { delete AbcFile; }

void FAbcImporter::UpdateAssetImportData(UDoodleAbcAssetImportData* AssetImportData) {
  AssetImportData->TrackNames.Empty();
  const TArray<FAbcPolyMesh*>& PolyMeshes = AbcFile->GetPolyMeshes();
  for (const FAbcPolyMesh* PolyMesh : PolyMeshes) {
    if (PolyMesh->bShouldImport) {
      AssetImportData->TrackNames.Add(PolyMesh->GetName());
    }
  }

  AssetImportData->NormalGenerationSettings = ImportSettings->NormalGenerationSettings;
  AssetImportData->CompressionSettings      = ImportSettings->CompressionSettings;
  AssetImportData->GeometryCacheSettings    = ImportSettings->GeometryCacheSettings;
  AssetImportData->ConversionSettings       = ImportSettings->ConversionSettings;
}

void FAbcImporter::RetrieveAssetImportData(UDoodleAbcAssetImportData* AssetImportData) {
  bool bAnySetForImport = false;

  for (FAbcPolyMesh* PolyMesh : AbcFile->GetPolyMeshes()) {
    if (AssetImportData->TrackNames.Contains(PolyMesh->GetName())) {
      PolyMesh->bShouldImport = true;
      bAnySetForImport        = true;
    } else {
      PolyMesh->bShouldImport = false;
    }
  }

  // If none were set to import, set all of them to import (probably different scene/setup)
  if (!bAnySetForImport) {
    for (FAbcPolyMesh* PolyMesh : AbcFile->GetPolyMeshes()) {
      PolyMesh->bShouldImport = true;
    }
  }
}

const EAbcImportError FAbcImporter::OpenAbcFileForImport(const FString InFilePath) {
  AbcFile = new FAbcFile(InFilePath);
  return AbcFile->Open();
}

const EAbcImportError FAbcImporter::ImportTrackData(
    const int32 InNumThreads, UDoodleAbcImportSettings* InImportSettings
) {
  ImportSettings             = InImportSettings;
  ImportSettings->NumThreads = InNumThreads;
  EAbcImportError Error      = AbcFile->Import(ImportSettings);

  return Error;
}

template <typename T>
T* FAbcImporter::CreateObjectInstance(UObject*& InParent, const FString& ObjectName, const EObjectFlags Flags) {
  // Parent package to place new mesh
  UPackage* Package = nullptr;
  FString NewPackageName;

  // Setup package name and create one accordingly
  NewPackageName = FPackageName::GetLongPackagePath(InParent->GetOutermost()->GetName() + TEXT("/") + ObjectName);
  NewPackageName = UPackageTools::SanitizePackageName(NewPackageName);
  Package        = CreatePackage(*NewPackageName);

  const FString SanitizedObjectName = ObjectTools::SanitizeObjectName(ObjectName);

  T* ExistingTypedObject            = FindObject<T>(Package, *SanitizedObjectName);
  UObject* ExistingObject           = FindObject<UObject>(Package, *SanitizedObjectName);

  if (ExistingTypedObject != nullptr) {
    ExistingTypedObject->PreEditChange(nullptr);
  } else if (ExistingObject != nullptr) {
    // Replacing an object.  Here we go!
    // Delete the existing object
    const bool bDeleteSucceeded = ObjectTools::DeleteSingleObject(ExistingObject);

    if (bDeleteSucceeded) {
      // Force GC so we can cleanly create a new asset (and not do an 'in place' replacement)
      CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

      // Create a package for each mesh
      Package  = CreatePackage(*NewPackageName);
      InParent = Package;
    } else {
      // failed to delete
      return nullptr;
    }
  }

  return NewObject<T>(Package, FName(*SanitizedObjectName), Flags | RF_Public);
}

UStaticMesh* FAbcImporter::CreateStaticMeshFromSample(
    UObject* InParent, const FString& Name, EObjectFlags Flags, const uint32 NumMaterials,
    const TArray<FString>& FaceSetNames, const FAbcMeshSample* Sample
) {
  UStaticMesh* StaticMesh = CreateObjectInstance<UStaticMesh>(InParent, Name, Flags);

  // Only import data if a valid object was created
  if (StaticMesh) {
    // Add the first LOD, we only support one
    int32 LODIndex = 0;
    StaticMesh->AddSourceModel();
    FMeshDescription* MeshDescription = StaticMesh->CreateMeshDescription(LODIndex);
    // Generate a new lighting GUID (so its unique)
    StaticMesh->SetLightingGuid();

    // Set it to use textured lightmaps. Note that Build Lighting will do the error-checking (texcoord index exists for
    // all LODs, etc).
    StaticMesh->SetLightMapResolution(64);
    StaticMesh->SetLightMapCoordinateIndex(1);

    // Material setup, since there isn't much material information in the Alembic file,
    UMaterial* DefaultMaterial = UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface);
    check(DefaultMaterial);

    // Material list
    StaticMesh->GetStaticMaterials().Empty();
    // If there were FaceSets available in the Alembic file use the number of unique face sets as num material entries,
    // otherwise default to one material for the whole mesh
    const uint32 FrameIndex = 0;
    uint32 NumFaceSets      = FaceSetNames.Num();

    for (uint32 MaterialIndex = 0; MaterialIndex < ((NumMaterials != 0) ? NumMaterials : 1); ++MaterialIndex) {
      UMaterialInterface* Material = nullptr;
      if (FaceSetNames.IsValidIndex(MaterialIndex)) {
        Material = AbcImporterUtilities::RetrieveMaterial(*AbcFile, FaceSetNames[MaterialIndex], InParent, Flags);

        if (Material != UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface)) {
          Material->PostEditChange();
        }
      }

      StaticMesh->GetStaticMaterials().Add((Material != nullptr) ? Material : DefaultMaterial);
    }

    GenerateMeshDescriptionFromSample(Sample, MeshDescription, StaticMesh);

    // Get the first LOD for filling it up with geometry, only support one LOD
    FStaticMeshSourceModel& SrcModel            = StaticMesh->GetSourceModel(LODIndex);
    // Set build settings for the static mesh
    SrcModel.BuildSettings.bRecomputeNormals    = false;
    SrcModel.BuildSettings.bRecomputeTangents   = false;
    SrcModel.BuildSettings.bUseMikkTSpace       = false;
    // Generate Lightmaps uvs (no support for importing right now)
    SrcModel.BuildSettings.bGenerateLightmapUVs = false;
    // Set lightmap UV index to 1 since we currently only import one set of UVs from the Alembic Data file
    SrcModel.BuildSettings.DstLightmapIndex     = 1;

    // Store the mesh description
    StaticMesh->CommitMeshDescription(LODIndex);

    // Set the Imported version before calling the build
    StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

    // Build the static mesh (using the build setting etc.) this generates correct tangents using the extracting
    // smoothing group along with the imported Normals data
    StaticMesh->Build(false);

    // No collision generation for now
    StaticMesh->CreateBodySetup();
  }

  return StaticMesh;
}

UGeometryCache* FAbcImporter::ImportAsGeometryCache(UObject* InParent, EObjectFlags Flags) {
  // Create a GeometryCache instance
  UGeometryCache* GeometryCache = CreateObjectInstance<UGeometryCache>(
      InParent,
      InParent != GetTransientPackage()
          ? FPaths::GetBaseFilename(InParent->GetName())
          : (FPaths::GetBaseFilename(AbcFile->GetFilePath()) + "_" + FGuid::NewGuid().ToString()),
      Flags
  );

  // Only import data if a valid object was created
  if (GeometryCache) {
    TArray<TUniquePtr<FComponentReregisterContext>> ReregisterContexts;
    for (TObjectIterator<UGeometryCacheComponent> CacheIt; CacheIt; ++CacheIt) {
      if (CacheIt->GetGeometryCache() == GeometryCache) {
        ReregisterContexts.Add(MakeUnique<FComponentReregisterContext>(*CacheIt));
      }
    }

    // In case this is a reimport operation
    GeometryCache->ClearForReimporting();

    // Load the default material for later usage
    UMaterial* DefaultMaterial = UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface);
    check(DefaultMaterial);
    uint32 MaterialOffset     = 0;

    // Add tracks
    const int32 NumPolyMeshes = AbcFile->GetNumPolyMeshes();
    if (NumPolyMeshes != 0) {
      TArray<UGeometryCacheTrackStreamable*> Tracks;

      TArray<FAbcPolyMesh*> ImportPolyMeshes;
      TArray<int32> MaterialOffsets;

      const bool bContainsHeterogeneousMeshes = AbcFile->ContainsHeterogeneousMeshes();
      if (ImportSettings->GeometryCacheSettings.bApplyConstantTopologyOptimizations && bContainsHeterogeneousMeshes) {
        TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(
            EMessageSeverity::Warning, LOCTEXT(
                                           "HeterogeneousMeshesAndForceSingle",
                                           "Unable to enforce constant topology optimizations as the imported tracks "
                                           "contain topology varying data."
                                       )
        );
        FAbcImportLogger::AddImportMessage(Message);
      }

      if (ImportSettings->GeometryCacheSettings.bFlattenTracks) {
        // UGeometryCacheCodecRaw* Codec = NewObject<UGeometryCacheCodecRaw>(GeometryCache,
        // FName(*FString(TEXT("Flattened_Codec"))), RF_Public);
        UGeometryCacheCodecV1* Codec =
            NewObject<UGeometryCacheCodecV1>(GeometryCache, FName(*FString(TEXT("Flattened_Codec"))), RF_Public);
        Codec->InitializeEncoder(
            ImportSettings->GeometryCacheSettings.CompressedPositionPrecision,
            ImportSettings->GeometryCacheSettings.CompressedTextureCoordinatesNumberOfBits
        );
        UGeometryCacheTrackStreamable* Track = NewObject<UGeometryCacheTrackStreamable>(
            GeometryCache, FName(*FString(TEXT("Flattened_Track"))), RF_Public
        );

        const bool bCalculateMotionVectors = false;
        Track->BeginCoding(
            Codec,
            ImportSettings->GeometryCacheSettings.bApplyConstantTopologyOptimizations && !bContainsHeterogeneousMeshes,
            bCalculateMotionVectors, ImportSettings->GeometryCacheSettings.bOptimizeIndexBuffers
        );
        Tracks.Add(Track);

        FScopedSlowTask SlowTask(
            (ImportSettings->SamplingSettings.FrameEnd + 1) - ImportSettings->SamplingSettings.FrameStart,
            FText::FromString(FString(TEXT("Importing Frames")))
        );
        SlowTask.MakeDialog(true);

        const TArray<FString>& UniqueFaceSetNames  = AbcFile->GetUniqueFaceSetNames();
        const TArray<FAbcPolyMesh*>& PolyMeshes    = AbcFile->GetPolyMeshes();

        const int32 NumTracks                      = Tracks.Num();
        int32 PreviousNumVertices                  = 0;
        TFunction<void(int32, FAbcFile*)> Callback = [this, &Tracks, &SlowTask, &UniqueFaceSetNames, &PolyMeshes,
                                                      &PreviousNumVertices](
                                                         int32 FrameIndex, const FAbcFile* InAbcFile
                                                     ) {
          const bool bUseVelocitiesAsMotionVectors = false;
          FGeometryCacheMeshData MeshData;
          bool bConstantTopology                 = true;
          const bool bStoreImportedVertexNumbers = ImportSettings->GeometryCacheSettings.bStoreImportedVertexNumbers;

          AbcImporterUtilities::MergePolyMeshesToMeshData(
              FrameIndex, ImportSettings->SamplingSettings.FrameStart, AbcFile->GetSecondsPerFrame(),
              bUseVelocitiesAsMotionVectors, PolyMeshes, UniqueFaceSetNames, MeshData, PreviousNumVertices,
              bConstantTopology, bStoreImportedVertexNumbers
          );

          Tracks[0]->AddMeshSample(
              MeshData, PolyMeshes[0]->GetTimeForFrameIndex(FrameIndex) - InAbcFile->GetImportTimeOffset(),
              bConstantTopology
          );

          if (IsInGameThread()) {
            SlowTask.EnterProgressFrame(1.0f);
          }
        };

        AbcFile->ProcessFrames(Callback, EFrameReadFlags::ApplyMatrix);

        // Now add materials for all the face set names
        for (const FString& FaceSetName : UniqueFaceSetNames) {
          UMaterialInterface* Material = AbcImporterUtilities::RetrieveMaterial(*AbcFile, FaceSetName, InParent, Flags);
          GeometryCache->Materials.Add((Material != nullptr) ? Material : DefaultMaterial);

          if (Material != UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface)) {
            Material->PostEditChange();
          }
        }
      } else {
        const TArray<FAbcPolyMesh*>& PolyMeshes = AbcFile->GetPolyMeshes();
        for (FAbcPolyMesh* PolyMesh : PolyMeshes) {
          if (PolyMesh->bShouldImport) {
            FName BaseName  = FName(*(PolyMesh->GetName()));
            // UGeometryCacheCodecRaw* Codec = NewObject<UGeometryCacheCodecRaw>(GeometryCache,
            // FName(*(PolyMesh->GetName() + FString(TEXT("_Codec")))), RF_Public);
            FName CodecName = MakeUniqueObjectName(
                GeometryCache, UGeometryCacheCodecV1::StaticClass(),
                FName(BaseName.ToString() + FString(TEXT("_Codec")))
            );
            UGeometryCacheCodecV1* Codec = NewObject<UGeometryCacheCodecV1>(GeometryCache, CodecName, RF_Public);
            Codec->InitializeEncoder(
                ImportSettings->GeometryCacheSettings.CompressedPositionPrecision,
                ImportSettings->GeometryCacheSettings.CompressedTextureCoordinatesNumberOfBits
            );

            FName TrackName =
                MakeUniqueObjectName(GeometryCache, UGeometryCacheTrackStreamable::StaticClass(), BaseName);
            UGeometryCacheTrackStreamable* Track =
                NewObject<UGeometryCacheTrackStreamable>(GeometryCache, TrackName, RF_Public);

            const bool bCalculateMotionVectors = false;
            Track->BeginCoding(
                Codec,
                ImportSettings->GeometryCacheSettings.bApplyConstantTopologyOptimizations &&
                    !bContainsHeterogeneousMeshes,
                bCalculateMotionVectors, ImportSettings->GeometryCacheSettings.bOptimizeIndexBuffers
            );

            ImportPolyMeshes.Add(PolyMesh);
            Tracks.Add(Track);
            MaterialOffsets.Add(MaterialOffset);

            // Add materials for this Mesh Object
            const uint32 NumMaterials = (PolyMesh->FaceSetNames.Num() > 0) ? PolyMesh->FaceSetNames.Num() : 1;
            for (uint32 MaterialIndex = 0; MaterialIndex < NumMaterials; ++MaterialIndex) {
              UMaterialInterface* Material = nullptr;
              if (PolyMesh->FaceSetNames.IsValidIndex(MaterialIndex)) {
                Material = AbcImporterUtilities::RetrieveMaterial(
                    *AbcFile, PolyMesh->FaceSetNames[MaterialIndex], InParent, Flags
                );
                if (Material != UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface)) {
                  Material->PostEditChange();
                }
              }

              GeometryCache->Materials.Add((Material != nullptr) ? Material : DefaultMaterial);
            }

            MaterialOffset += NumMaterials;
          }
        }

        const int32 NumTracks                      = Tracks.Num();
        TFunction<void(int32, FAbcFile*)> Callback = [this, NumTracks, &ImportPolyMeshes, &Tracks,
                                                      &MaterialOffsets](int32 FrameIndex, const FAbcFile* InAbcFile) {
          for (int32 TrackIndex = 0; TrackIndex < NumTracks; ++TrackIndex) {
            const FAbcPolyMesh* PolyMesh = ImportPolyMeshes[TrackIndex];
            if (PolyMesh->bShouldImport) {
              UGeometryCacheTrackStreamable* Track = Tracks[TrackIndex];

              // Generate the mesh data for this sample
              const bool bVisible                  = PolyMesh->GetVisibility(FrameIndex);
              const float FrameTime = PolyMesh->GetTimeForFrameIndex(FrameIndex) - InAbcFile->GetImportTimeOffset();
              if (bVisible) {
                const bool bUseVelocitiesAsMotionVectors = false;
                const bool bStoreImportedVertexData = ImportSettings->GeometryCacheSettings.bStoreImportedVertexNumbers;
                const FAbcMeshSample* Sample        = PolyMesh->GetSample(FrameIndex);
                FGeometryCacheMeshData MeshData;
                AbcImporterUtilities::GeometryCacheDataForMeshSample(
                    MeshData, Sample, MaterialOffsets[TrackIndex], AbcFile->GetSecondsPerFrame(),
                    bUseVelocitiesAsMotionVectors, bStoreImportedVertexData
                );
                Track->AddMeshSample(MeshData, FrameTime, PolyMesh->bConstantTopology);
              }

              Track->AddVisibilitySample(bVisible, FrameTime);
            }
          }
        };

        AbcFile->ProcessFrames(Callback, EFrameReadFlags::ApplyMatrix);
      }

      TArray<FMatrix> Mats;
      Mats.Add(FMatrix::Identity);
      Mats.Add(FMatrix::Identity);

      for (UGeometryCacheTrackStreamable* Track : Tracks) {
        TArray<float> MatTimes;
        MatTimes.Add(0.0f);
        MatTimes.Add(AbcFile->GetImportLength() + AbcFile->GetImportTimeOffset());
        Track->SetMatrixSamples(Mats, MatTimes);

        // 有些轨道可能没有任何网格样本，因为它们是不可见的（在bFlattenTracks=false的情况下可能发生），所以跳过它们。

        Track->EndCoding();
        GeometryCache->AddTrack(Track);
      }
    }

    // For alembic, for now, we define the duration of the tracks as the duration of the longer track in the whole file
    // so all tracks loop in union
    float MaxDuration = 0.0f;
    for (auto Track : GeometryCache->Tracks) {
      MaxDuration = FMath::Max(MaxDuration, Track->GetDuration());
    }
    for (auto Track : GeometryCache->Tracks) {
      Track->SetDuration(MaxDuration);
    }
    // Also store the number of frames in the cache
    GeometryCache->SetFrameStartEnd(
        ImportSettings->SamplingSettings.FrameStart, ImportSettings->SamplingSettings.FrameEnd
    );

    // Update all geometry cache components, TODO move render-data from component to GeometryCache and allow for DDC
    // population
    for (TObjectIterator<UGeometryCacheComponent> CacheIt; CacheIt; ++CacheIt) {
      CacheIt->OnObjectReimported(GeometryCache);
    }

    SetMetaData({GeometryCache});
  }

  return GeometryCache;
}

void FAbcImporter::SetupMorphTargetCurves(
    USkeleton* Skeleton, FName ConstCurveName, UAnimSequence* Sequence, const TArray<float>& CurveValues,
    const TArray<float>& TimeValues
) {
  FSmartName NewName;
  Skeleton->AddSmartNameAndModify(USkeleton::AnimCurveMappingName, ConstCurveName, NewName);

  FFloatCurve* NewCurve =
      static_cast<FFloatCurve*>(Sequence->RawCurveData.GetCurveData(NewName.UID, ERawCurveTrackTypes::RCT_Float));
  ensure(NewCurve);

  FRichCurve RichCurve;
  for (int32 KeyIndex = 0; KeyIndex < CurveValues.Num(); ++KeyIndex) {
    const float CurveValue                           = CurveValues[KeyIndex];
    const float TimeValue                            = TimeValues[KeyIndex];

    FKeyHandle NewKeyHandle                          = RichCurve.AddKey(TimeValue, CurveValue, false);

    ERichCurveInterpMode NewInterpMode               = RCIM_Linear;
    ERichCurveTangentMode NewTangentMode             = RCTM_Auto;
    ERichCurveTangentWeightMode NewTangentWeightMode = RCTWM_WeightedNone;

    RichCurve.SetKeyInterpMode(NewKeyHandle, NewInterpMode);
    RichCurve.SetKeyTangentMode(NewKeyHandle, NewTangentMode);
    RichCurve.SetKeyTangentWeightMode(NewKeyHandle, NewTangentWeightMode);
  }
}

void FAbcImporter::SetMetaData(const TArray<UObject*>& Objects) {
  const TArray<FAbcFile::FMetaData> ArchiveMetaData = AbcFile->GetArchiveMetaData();
  for (UObject* Object : Objects) {
    if (Object) {
      for (const FAbcFile::FMetaData& MetaData : ArchiveMetaData) {
        Object->GetOutermost()->GetMetaData()->SetValue(Object, *MetaData.Key, *MetaData.Value);
      }
    }
  }
}

UGeometryCache* FAbcImporter::ReimportAsGeometryCache(UGeometryCache* GeometryCache) {
  UGeometryCache* ReimportedCache = ImportAsGeometryCache(GeometryCache->GetOuter(), RF_Public | RF_Standalone);
  return ReimportedCache;
}

const TArray<FAbcPolyMesh*>& FAbcImporter::GetPolyMeshes() const { return AbcFile->GetPolyMeshes(); }

const uint32 FAbcImporter::GetStartFrameIndex() const { return (AbcFile != nullptr) ? AbcFile->GetMinFrameIndex() : 0; }

const uint32 FAbcImporter::GetEndFrameIndex() const {
  return (AbcFile != nullptr) ? FMath::Max(AbcFile->GetMaxFrameIndex() - 1, 1) : 1;
}

const uint32 FAbcImporter::GetNumMeshTracks() const { return (AbcFile != nullptr) ? AbcFile->GetNumPolyMeshes() : 0; }

void FAbcImporter::GenerateMeshDescriptionFromSample(
    const FAbcMeshSample* Sample, FMeshDescription* MeshDescription, UStaticMesh* StaticMesh
) {
  if (MeshDescription == nullptr) {
    return;
  }

  FStaticMeshAttributes Attributes(*MeshDescription);

  TVertexAttributesRef<FVector> VertexPositions = Attributes.GetVertexPositions();
  TEdgeAttributesRef<bool> EdgeHardnesses       = Attributes.GetEdgeHardnesses();
  TPolygonGroupAttributesRef<FName> PolygonGroupImportedMaterialSlotNames =
      Attributes.GetPolygonGroupMaterialSlotNames();
  TVertexInstanceAttributesRef<FVector> VertexInstanceNormals     = Attributes.GetVertexInstanceNormals();
  TVertexInstanceAttributesRef<FVector> VertexInstanceTangents    = Attributes.GetVertexInstanceTangents();
  TVertexInstanceAttributesRef<float> VertexInstanceBinormalSigns = Attributes.GetVertexInstanceBinormalSigns();
  TVertexInstanceAttributesRef<FVector4> VertexInstanceColors     = Attributes.GetVertexInstanceColors();
  TVertexInstanceAttributesRef<FVector2D> VertexInstanceUVs       = Attributes.GetVertexInstanceUVs();

  // Speedtree use UVs to store is data
  VertexInstanceUVs.SetNumIndices(Sample->NumUVSets);

  for (int32 MatIndex = 0; MatIndex < StaticMesh->GetStaticMaterials().Num(); ++MatIndex) {
    const FPolygonGroupID PolygonGroupID = MeshDescription->CreatePolygonGroup();
    PolygonGroupImportedMaterialSlotNames[PolygonGroupID] =
        StaticMesh->GetStaticMaterials()[MatIndex].ImportedMaterialSlotName;
  }

  // position
  for (int32 VertexIndex = 0; VertexIndex < Sample->Vertices.Num(); ++VertexIndex) {
    FVector Position          = Sample->Vertices[VertexIndex];

    FVertexID VertexID        = MeshDescription->CreateVertex();
    VertexPositions[VertexID] = FVector(Position);
  }

  uint32 VertexIndices[3];
  uint32 TriangleCount = Sample->Indices.Num() / 3;
  for (uint32 TriangleIndex = 0; TriangleIndex < TriangleCount; ++TriangleIndex) {
    const uint32 IndiceIndex0 = TriangleIndex * 3;
    VertexIndices[0]          = Sample->Indices[IndiceIndex0];
    VertexIndices[1]          = Sample->Indices[IndiceIndex0 + 1];
    VertexIndices[2]          = Sample->Indices[IndiceIndex0 + 2];

    // Skip degenerated triangle
    if (VertexIndices[0] == VertexIndices[1] || VertexIndices[1] == VertexIndices[2] ||
        VertexIndices[0] == VertexIndices[2]) {
      continue;
    }

    TArray<FVertexInstanceID> CornerVertexInstanceIDs;
    CornerVertexInstanceIDs.SetNum(3);
    FVertexID CornerVertexIDs[3];
    for (int32 Corner = 0; Corner < 3; ++Corner) {
      uint32 IndiceIndex = IndiceIndex0 + Corner;
      uint32 VertexIndex = VertexIndices[Corner];
      const FVertexID VertexID(VertexIndex);
      const FVertexInstanceID VertexInstanceID      = MeshDescription->CreateVertexInstance(VertexID);

      // tangents
      FVector TangentX                              = Sample->TangentX[IndiceIndex];
      FVector TangentY                              = Sample->TangentY[IndiceIndex];
      FVector TangentZ                              = Sample->Normals[IndiceIndex];

      VertexInstanceTangents[VertexInstanceID]      = TangentX;
      VertexInstanceNormals[VertexInstanceID]       = TangentZ;
      VertexInstanceBinormalSigns[VertexInstanceID] = GetBasisDeterminantSign(
          (FVector)TangentX.GetSafeNormal(), (FVector)TangentY.GetSafeNormal(), (FVector)TangentZ.GetSafeNormal()
      );

      if (Sample->Colors.Num()) {
        VertexInstanceColors[VertexInstanceID] = FVector4(Sample->Colors[IndiceIndex]);
      } else {
        VertexInstanceColors[VertexInstanceID] = FVector4(FLinearColor::White);
      }

      for (uint32 UVIndex = 0; UVIndex < Sample->NumUVSets; ++UVIndex) {
        VertexInstanceUVs.Set(VertexInstanceID, UVIndex, Sample->UVs[UVIndex][IndiceIndex]);
      }
      CornerVertexInstanceIDs[Corner] = VertexInstanceID;
      CornerVertexIDs[Corner]         = VertexID;
    }

    const FPolygonGroupID PolygonGroupID(Sample->MaterialIndices[TriangleIndex]);
    // Insert a polygon into the mesh
    MeshDescription->CreatePolygon(PolygonGroupID, CornerVertexInstanceIDs);
  }
  // Set the edge hardness from the smooth group
  FStaticMeshOperations::ConvertSmoothGroupToHardEdges(Sample->SmoothingGroupIndices, *MeshDescription);
}

bool FAbcImporter::BuildSkeletalMesh(
    FSkeletalMeshLODModel& LODModel, const FReferenceSkeleton& RefSkeleton, FAbcMeshSample* Sample,
    TArray<int32>& OutMorphTargetVertexRemapping, TArray<int32>& OutUsedVertexIndicesForMorphs
) {
  // Module manager is not thread safe, so need to prefetch before parallelfor
  IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");

  const bool bComputeNormals    = (Sample->Normals.Num() == 0);
  const bool bComputeTangents   = (Sample->TangentX.Num() == 0) || (Sample->TangentY.Num() == 0);

  // Compute normals/tangents if needed
  if (bComputeNormals || bComputeTangents) {
    uint32 TangentOptions = 0;
    MeshUtilities.CalculateTangents(
        Sample->Vertices, Sample->Indices, Sample->UVs[0], Sample->SmoothingGroupIndices, TangentOptions,
        Sample->TangentX, Sample->TangentY, Sample->Normals
    );
  }

  // Populate faces
  const uint32 NumFaces = Sample->Indices.Num() / 3;
  TArray<SkeletalMeshImportData::FMeshFace> Faces;
  Faces.AddZeroed(NumFaces);

  TArray<FMeshSection> MeshSections;
  MeshSections.AddDefaulted(Sample->NumMaterials);

  // Process all the faces and add to their respective mesh section
  for (uint32 FaceIndex = 0; FaceIndex < NumFaces; ++FaceIndex) {
    const uint32 FaceOffset   = FaceIndex * 3;
    const int32 MaterialIndex = Sample->MaterialIndices[FaceIndex];

    check(MeshSections.IsValidIndex(MaterialIndex));

    FMeshSection& Section = MeshSections[MaterialIndex];
    Section.MaterialIndex = MaterialIndex;
    Section.NumUVSets     = Sample->NumUVSets;

    for (uint32 VertexIndex = 0; VertexIndex < 3; ++VertexIndex) {
      LODModel.MaxImportVertex = FMath::Max<int32>(LODModel.MaxImportVertex, Sample->Indices[FaceOffset + VertexIndex]);

      Section.OriginalIndices.Add(FaceOffset + VertexIndex);
      Section.Indices.Add(Sample->Indices[FaceOffset + VertexIndex]);
      Section.TangentX.Add((FVector)Sample->TangentX[FaceOffset + VertexIndex]);
      Section.TangentY.Add((FVector)Sample->TangentY[FaceOffset + VertexIndex]);
      Section.TangentZ.Add((FVector)Sample->Normals[FaceOffset + VertexIndex]);

      for (uint32 UVIndex = 0; UVIndex < Sample->NumUVSets; ++UVIndex) {
        Section.UVs[UVIndex].Add(FVector2D(Sample->UVs[UVIndex][FaceOffset + VertexIndex]));
      }

      Section.Colors.Add(Sample->Colors[FaceOffset + VertexIndex].ToFColor(false));
    }

    ++Section.NumFaces;
  }

  // Sort the vertices by z value
  MeshSections.Sort([](const FMeshSection& A, const FMeshSection& B) { return A.MaterialIndex < B.MaterialIndex; });

  // Create Skeletal mesh LOD sections
  LODModel.Sections.Empty(MeshSections.Num());
  LODModel.NumVertices = 0;
  LODModel.IndexBuffer.Empty();
  TArray<uint32> RawPointIndices;

  TArray<TArray<uint32>> VertexIndexRemap;
  VertexIndexRemap.Empty(MeshSections.Num());

  // Create actual skeletal mesh sections
  for (int32 SectionIndex = 0; SectionIndex < MeshSections.Num(); ++SectionIndex) {
    const FMeshSection& SourceSection     = MeshSections[SectionIndex];
    FSkelMeshSection& TargetSection       = *new (LODModel.Sections) FSkelMeshSection();
    TargetSection.MaterialIndex           = (uint16)SourceSection.MaterialIndex;
    TargetSection.NumTriangles            = SourceSection.NumFaces;
    TargetSection.BaseVertexIndex         = LODModel.NumVertices;

    // Separate the section's vertices into rigid and soft vertices.
    TArray<uint32>& ChunkVertexIndexRemap = *new (VertexIndexRemap) TArray<uint32>();
    ChunkVertexIndexRemap.AddUninitialized(SourceSection.NumFaces * 3);

    TMultiMap<uint32, uint32> FinalVertices;
    TArray<uint32> DuplicateVertexIndices;

    // Reused soft vertex
    FSoftSkinVertex NewVertex;

    uint32 VertexOffset = 0;
    // Generate Soft Skin vertices (used by the skeletal mesh)
    for (uint32 FaceIndex = 0; FaceIndex < SourceSection.NumFaces; ++FaceIndex) {
      const uint32 FaceOffset = FaceIndex * 3;

      for (uint32 VertexIndex = 0; VertexIndex < 3; ++VertexIndex) {
        const uint32 Index = SourceSection.Indices[FaceOffset + VertexIndex];

        DuplicateVertexIndices.Reset();
        FinalVertices.MultiFind(Index, DuplicateVertexIndices);

        // Populate vertex data
        NewVertex.Position = Sample->Vertices[Index];
        NewVertex.TangentX = (FVector)SourceSection.TangentX[FaceOffset + VertexIndex];
        NewVertex.TangentY = (FVector)SourceSection.TangentY[FaceOffset + VertexIndex];
        NewVertex.TangentZ = (FVector)SourceSection.TangentZ[FaceOffset + VertexIndex];  // LWC_TODO: precision loss
        for (uint32 UVIndex = 0; UVIndex < SourceSection.NumUVSets; ++UVIndex) {
          NewVertex.UVs[UVIndex] = FVector2D(SourceSection.UVs[UVIndex][FaceOffset + VertexIndex]);
        }

        NewVertex.Color = SourceSection.Colors[FaceOffset + VertexIndex];

        // Set up bone influence (only using one bone so maxed out weight)
        FMemory::Memzero(NewVertex.InfluenceBones);
        FMemory::Memzero(NewVertex.InfluenceWeights);
        NewVertex.InfluenceWeights[0] = 255;

        int32 FinalVertexIndex        = INDEX_NONE;
        if (DuplicateVertexIndices.Num()) {
          for (const uint32 DuplicateVertexIndex : DuplicateVertexIndices) {
            if (AbcImporterUtilities::AreVerticesEqual(TargetSection.SoftVertices[DuplicateVertexIndex], NewVertex)) {
              // Use the existing vertex
              FinalVertexIndex = DuplicateVertexIndex;
              break;
            }
          }
        }

        if (FinalVertexIndex == INDEX_NONE) {
          FinalVertexIndex = TargetSection.SoftVertices.Add(NewVertex);
#if PRINT_UNIQUE_VERTICES
          FPlatformMisc::LowLevelOutputDebugStringf(
              TEXT("Vert - P(%.2f, %.2f,%.2f) N(%.2f, %.2f,%.2f) TX(%.2f, %.2f,%.2f) TY(%.2f, %.2f,%.2f) UV(%.2f, "
                   "%.2f)\n"),
              NewVertex.Position.X, NewVertex.Position.Y, NewVertex.Position.Z,
              SourceSection.TangentX[FaceOffset + VertexIndex].X, SourceSection.TangentZ[FaceOffset + VertexIndex].X,
              SourceSection.TangentZ[FaceOffset + VertexIndex].Y, SourceSection.TangentZ[FaceOffset + VertexIndex].Z,
              SourceSection.TangentX[FaceOffset + VertexIndex].Y, SourceSection.TangentX[FaceOffset + VertexIndex].Z,
              SourceSection.TangentY[FaceOffset + VertexIndex].X, SourceSection.TangentY[FaceOffset + VertexIndex].Y,
              SourceSection.TangentY[FaceOffset + VertexIndex].Z, NewVertex.UVs[0].X, NewVertex.UVs[0].Y
          );
#endif

          FinalVertices.Add(Index, FinalVertexIndex);
          OutUsedVertexIndicesForMorphs.Add(Index);
          OutMorphTargetVertexRemapping.Add(SourceSection.OriginalIndices[FaceOffset + VertexIndex]);
        }

        RawPointIndices.Add(FinalVertexIndex);
        ChunkVertexIndexRemap[VertexOffset] = TargetSection.BaseVertexIndex + FinalVertexIndex;
        ++VertexOffset;
      }
    }

    LODModel.NumVertices += TargetSection.SoftVertices.Num();
    TargetSection.NumVertices = TargetSection.SoftVertices.Num();

    // Only need first bone from active bone indices
    TargetSection.BoneMap.Add(0);

    TargetSection.CalcMaxBoneInfluences();
    TargetSection.CalcUse16BitBoneIndex();
  }

  // Only using bone zero
  LODModel.ActiveBoneIndices.Add(0);

  // Copy raw point indices to LOD model.
  LODModel.RawPointIndices.RemoveBulkData();
  if (RawPointIndices.Num()) {
    LODModel.RawPointIndices.Lock(LOCK_READ_WRITE);
    void* Dest = LODModel.RawPointIndices.Realloc(RawPointIndices.Num());
    FMemory::Memcpy(Dest, RawPointIndices.GetData(), LODModel.RawPointIndices.GetBulkDataSize());
    LODModel.RawPointIndices.Unlock();
  }

  // Finish building the sections.
  for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); SectionIndex++) {
    FSkelMeshSection& Section                     = LODModel.Sections[SectionIndex];

    const TArray<uint32>& SectionIndices          = MeshSections[SectionIndex].Indices;
    Section.BaseIndex                             = LODModel.IndexBuffer.Num();
    const int32 NumIndices                        = SectionIndices.Num();
    const TArray<uint32>& SectionVertexIndexRemap = VertexIndexRemap[SectionIndex];
    for (int32 Index = 0; Index < NumIndices; Index++) {
      uint32 VertexIndex = SectionVertexIndexRemap[Index];
      LODModel.IndexBuffer.Add(VertexIndex);
    }
  }

  // Compute the required bones for this model.
  USkeletalMesh::CalculateRequiredBones(LODModel, RefSkeleton, NULL);

  return true;
}

void FAbcImporter::GenerateMorphTargetVertices(
    FAbcMeshSample* BaseSample, TArray<FMorphTargetDelta>& MorphDeltas, FAbcMeshSample* AverageSample,
    uint32 WedgeOffset, const TArray<int32>& RemapIndices, const TArray<int32>& UsedVertexIndicesForMorphs,
    const uint32 VertexOffset, const uint32 IndexOffset
) {
  FMorphTargetDelta MorphVertex;
  const uint32 NumberOfUsedVertices = UsedVertexIndicesForMorphs.Num();
  for (uint32 VertIndex = 0; VertIndex < NumberOfUsedVertices; ++VertIndex) {
    const int32 UsedVertexIndex  = UsedVertexIndicesForMorphs[VertIndex] - VertexOffset;
    const uint32 UsedNormalIndex = RemapIndices[VertIndex] - IndexOffset;

    if (UsedVertexIndex >= 0 && UsedVertexIndex < BaseSample->Vertices.Num()) {
      // Position delta
      MorphVertex.PositionDelta = BaseSample->Vertices[UsedVertexIndex] - AverageSample->Vertices[UsedVertexIndex];
      // Tangent delta
      MorphVertex.TangentZDelta = BaseSample->Normals[UsedNormalIndex] - AverageSample->Normals[UsedNormalIndex];
      // Index of base mesh vert this entry is to modify
      MorphVertex.SourceIdx     = VertIndex;
      MorphDeltas.Add(MorphVertex);
    }
  }
}

FCompressedAbcData::~FCompressedAbcData() {
  delete AverageSample;
  for (FAbcMeshSample* Sample : BaseSamples) {
    delete Sample;
  }
}

#undef LOCTEXT_NAMESPACE
