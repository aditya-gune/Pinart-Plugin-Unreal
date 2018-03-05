// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "ModuleManager.h"
#include <vector>
#include "MyActor.h"

class FToolBarBuilder;
class FMenuBuilder;

class FEditorWindowModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void GetNameFromTextInput(const FText & Text);
	int LoadImageFromPath(const FString & Path);
	void GetHeightMap();
	void SpawnActor(FVector SpawnLocation, FVector scale);
	IImageWrapperPtr CreateImageWrapper(IImageWrapperModule& ImageWrapperModule, const FString& Path);

	UTexture2D * LoadTextureFromPath(const FString & Path);
	void AddMenuExtension(FMenuBuilder& Builder);

	FString Value;	//holds string values from window input
	UTexture2D* imageTexture;	//holds texture2D
	
	UPROPERTY(EditAnywhere, Category = "Globals")
	static FVector scale;

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};