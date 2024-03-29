// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Doodle/Abc/DoodleAbcImportSettings.h"
#include "EditorFramework/AssetImportData.h"
//
#include "DoodleAbcAssetImportData.generated.h"

/**
 * Base class for import data and options used when importing any asset from Alembic
 */
UCLASS()
class UDoodleAbcAssetImportData : public UAssetImportData {
  GENERATED_UCLASS_BODY()
 public:
  UPROPERTY()
  TArray<FString> TrackNames;

  UPROPERTY()
  FDoodleAbcNormalGenerationSettings NormalGenerationSettings;

  UPROPERTY()
  FDoodleAbcCompressionSettings CompressionSettings;

  UPROPERTY()
  FDoodleAbcGeometryCacheSettings GeometryCacheSettings;

  UPROPERTY()
  FDoodleAbcConversionSettings ConversionSettings;
};
