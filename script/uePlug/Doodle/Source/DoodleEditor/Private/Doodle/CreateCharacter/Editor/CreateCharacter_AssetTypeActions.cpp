#include "CreateCharacter_AssetTypeActions.h"

#include "Doodle/CreateCharacter/CoreData/DoodleCreateCharacterConfig.h"
#include "Doodle/CreateCharacter/CreateCharacterMianUI.h"

#define LOCTEXT_NAMESPACE "FAssetTypeActions_CreateCharacter"

FText FAssetTypeActions_CreateCharacter::GetName() const {
  return LOCTEXT("AssetTypeActions_MotionField", "Create Character");
}

FColor FAssetTypeActions_CreateCharacter::GetTypeColor() const {
  return FColor::Magenta;
}

UClass* FAssetTypeActions_CreateCharacter::GetSupportedClass() const {
  return UDoodleCreateCharacterConfig::StaticClass();
}

void FAssetTypeActions_CreateCharacter::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) {
  const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

  for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt) {
    if (UDoodleCreateCharacterConfig* MotionField = Cast<UDoodleCreateCharacterConfig>(*ObjIt)) {
      TSharedRef L_Ref = MakeShared<FCreateCharacterMianUI>();
      L_Ref->InitCreateCharacterMianUI(Mode, EditWithinLevelEditor, MotionField);
      // TSharedRef<FMotionFieldEditor> NewFlipbookEditor(new FMotionFieldEditor());
      // NewFlipbookEditor->InitMotionFieldEditor(Mode, EditWithinLevelEditor, MotionField);
    }
  }
}

uint32 FAssetTypeActions_CreateCharacter::GetCategories() {
  return AssType;
}
#undef LOCTEXT_NAMESPACE