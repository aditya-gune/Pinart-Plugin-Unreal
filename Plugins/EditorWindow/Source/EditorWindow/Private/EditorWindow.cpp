// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "EditorWindow.h"
#include "EditorWindowStyle.h"
#include "EditorWindowCommands.h"


static const FName EditorWindowTabName("EditorWindow");

#define LOCTEXT_NAMESPACE "FEditorWindowModule"

void FEditorWindowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FEditorWindowStyle::Initialize();
	FEditorWindowStyle::ReloadTextures();

	FEditorWindowCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FEditorWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FEditorWindowModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FEditorWindowModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FEditorWindowModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(EditorWindowTabName, FOnSpawnTab::CreateRaw(this, &FEditorWindowModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FEditorWindowTabTitle", "EditorWindow"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FEditorWindowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorWindowStyle::Shutdown();

	FEditorWindowCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EditorWindowTabName);
}

TSharedRef<SDockTab> FEditorWindowModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FEditorWindowModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("EditorWindow.cpp"))
		);

	 

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SScrollBox)
			+ SScrollBox::Slot().Padding(10, 5)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().HAlign(HAlign_Fill)
				[
					
					SNew(SBox)
					[
						/*SNew(STextBlock)
						.Text(FText::FromString(TEXT("Name")))*/
						SNew(SEditableTextBox)
						.OnTextChanged_Raw(this, &FEditorWindowModule::GetNameFromTextInput)
					]
				]
		+ SVerticalBox::Slot().HAlign(HAlign_Fill)
			[
				
				SNew(SBox)
				[
					/*SNew(STextBlock)
					.Text(FText::FromString(TEXT("Image File")))*/
					SNew(SEditableTextBox)
					.OnTextChanged_Raw(this, &FEditorWindowModule::GetNameFromTextInput)
				]
			]
		+ SVerticalBox::Slot().HAlign(HAlign_Fill)
			[
				
				SNew(SBox)
				[
					/*SNew(STextBlock)
					.Text(FText::FromString(TEXT("Size")))*/
					SNew(SEditableTextBox)
					.OnTextChanged_Raw(this, &FEditorWindowModule::GetNameFromTextInput)

				]
			]	

		]
		
		
		];
	
}



void FEditorWindowModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(EditorWindowTabName);
}

void FEditorWindowModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FEditorWindowCommands::Get().OpenPluginWindow);
}

void FEditorWindowModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FEditorWindowCommands::Get().OpenPluginWindow);
}

void FEditorWindowModule::GetNameFromTextInput(const FText & Text) {
	FText TextForString = Text;
	FString str = TextForString.ToString();
	
	//update this->value with the string
	Value = str;
	FString PathToLoad = "/Game/Textures/YourStructureHere";
	LoadImageFromPath(Value);

}

//Sample file:
//D:\Aditya\Pictures\20170916_175749.jpg
//D:\Aditya\Pictures\rgbtest.jpg	
int FEditorWindowModule::LoadImageFromPath(const FString& Path)
{
	UE_LOG(LogTemp, Warning, TEXT("in LoadImageFromPath"));
	// Represents the entire file in memory.
	TArray<uint8> RawFileData;

	if (FFileHelper::LoadFileToArray(RawFileData, *Path))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadFileToArray OK"));
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		
		
		IImageWrapperPtr ImageWrapper = CreateImageWrapper(ImageWrapperModule, Path);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			UE_LOG(LogTemp, Warning, TEXT("ImageWrapper is valid"));
			const TArray<uint8>* UncompressedBGRA = NULL;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				UE_LOG(LogTemp, Warning, TEXT("can GetRaw"));

				// Create the UTexture for rendering
				UTexture2D* MyTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

				// Fill in the source data from the file
				void* TextureData = MyTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				MyTexture->PlatformData->Mips[0].BulkData.Unlock();

				// Update the rendering resource from data.
				MyTexture->UpdateResource();
				imageTexture = MyTexture;

				UE_LOG(LogTemp, Warning, TEXT("getting height map"));
				FTexture2DMipMap* MyMipMap = &MyTexture->PlatformData->Mips[0];

				UE_LOG(LogTemp, Warning, TEXT("MipMap: %s"), MyMipMap);

				FByteBulkData* RawImageData = &MyMipMap->BulkData;
				UE_LOG(LogTemp, Warning, TEXT("RawImageData: %s"), RawImageData);

				FColor* FormatedImageData = (FColor*)(RawImageData->Lock(LOCK_READ_ONLY));
				UE_LOG(LogTemp, Warning, TEXT("RGB:%u"), (uint8)FormatedImageData->R);

				int32 PixelX = 1;
				int32 PixelY = 1;
				int32 TextureWidth = MyMipMap->SizeX;
				int32 TextureHeight = MyMipMap->SizeY;
				FColor PixelColor;
				UE_LOG(LogTemp, Warning, TEXT("(X: %d, Y: %d)"), PixelX, PixelY);
				UE_LOG(LogTemp, Warning, TEXT("(max X: %d, max Y: %d)"), TextureWidth, TextureHeight);
				if (PixelX >= 0 && PixelX < TextureWidth && PixelY >= 0 && PixelY < TextureHeight)
				{
					PixelColor.R = (uint8)FormatedImageData[PixelY * TextureWidth + PixelX].R;
					PixelColor.G = (uint8)FormatedImageData[PixelY * TextureWidth + PixelX].G;
					PixelColor.B = (uint8)FormatedImageData[PixelY * TextureWidth + PixelX].B;
					PixelColor.A = (uint8)FormatedImageData[PixelY * TextureWidth + PixelX].A;
					UE_LOG(LogTemp, Warning, TEXT("(%d, %d) | R:%u G:%u B:%u"), PixelX, PixelY, PixelColor.R, PixelColor.G, PixelColor.B);
				}
			
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("GetRaw failed"));
			}
		}
		else
		{
			if (!ImageWrapper.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("ImageWrapper not valid"));
			}
			if(!ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
			{
				UE_LOG(LogTemp, Warning, TEXT("ImageWrapper->SetCompressed failed"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadFileToArray failed"));
	}
	return 0;
}

void FEditorWindowModule::GetHeightMap()
{
	

}

void FEditorWindowModule::SpawnActor()
{
	FActorSpawnParameters SpawnParams;

	// Get Actor Location to Spawn
	FVector SpawnLocation = FVector(0.0f, 0.0f, 0.0f);

	//Set a rotation
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	//Spawn the actor 
	AMyActor pin;
	pin.Spawn(SpawnLocation, SpawnRotation, SpawnParams);

}

UTexture2D* FEditorWindowModule::LoadTextureFromPath(const FString& Path)
{
	if (Path.IsEmpty()) return NULL;

	return Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Path)));
}
IImageWrapperPtr FEditorWindowModule::CreateImageWrapper(IImageWrapperModule& ImageWrapperModule, const FString& Path)
{
	
	if ((Path.Contains(TEXT(".jpg")), ESearchCase::IgnoreCase) || (Path.Contains(TEXT(".jpeg")), ESearchCase::IgnoreCase))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	if (Path.Contains(TEXT(".png")), ESearchCase::IgnoreCase)
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	if (Path.Contains(TEXT(".bmp")), ESearchCase::IgnoreCase)
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	return ImageWrapperModule.CreateImageWrapper(EImageFormat::Invalid);
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorWindowModule, EditorWindow)