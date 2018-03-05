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
		/*+ SVerticalBox::Slot().HAlign(HAlign_Fill)
			[
				
				SNew(SBox)
				[
					//SNew(STextBlock)
					//.Text(FText::FromString(TEXT("Image File")))
					SNew(SEditableTextBox)
					.OnTextChanged_Raw(this, &FEditorWindowModule::GetNameFromTextInput)
				]
			]*/
		/*+ SVerticalBox::Slot().HAlign(HAlign_Fill)
			[
				
				SNew(SBox)
				[
					//SNew(STextBlock)
					//.Text(FText::FromString(TEXT("Size")))
					SNew(SEditableTextBox)
					.OnTextChanged_Raw(this, &FEditorWindowModule::GetNameFromTextInput)

				]
			]*/	

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
	//Memory buffer for reading the file
	TArray<uint8> RawFileData;

	if (FFileHelper::LoadFileToArray(RawFileData, *Path))
	{
		//Imagewrappermodule to read file into memory
		UE_LOG(LogTemp, Warning, TEXT("LoadFileToArray OK"));
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		IImageWrapperPtr ImageWrapper = CreateImageWrapper(ImageWrapperModule, Path);
		
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			UE_LOG(LogTemp, Warning, TEXT("ImageWrapper is valid"));
			const TArray<uint8>* ImageRGBA = NULL;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, ImageRGBA))
			{
				UE_LOG(LogTemp, Warning, TEXT("can GetRaw"));

				//Create UTexture2D from the file
				UTexture2D* ImageTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

				//Fill in image data from the file
				void* TextureData = ImageTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, ImageRGBA->GetData(), ImageRGBA->Num());
				ImageTexture->PlatformData->Mips[0].BulkData.Unlock();

				//Update rendering resource, member variable
				ImageTexture->UpdateResource();
				imageTexture = ImageTexture;

				//Get heightmap in form of RGBA
				UE_LOG(LogTemp, Warning, TEXT("getting height map"));
				FTexture2DMipMap* MipMap = &ImageTexture->PlatformData->Mips[0];

				//Get raw image data as bytes
				FByteBulkData* RawImageData = &MipMap->BulkData;
				UE_LOG(LogTemp, Warning, TEXT("RawImageData: %s"), RawImageData);

				//Get image data vector from raw bytes
				FColor* ImageDataVector = (FColor*)(RawImageData->Lock(LOCK_READ_ONLY));
				UE_LOG(LogTemp, Warning, TEXT("RGB:%u"), (uint8)ImageDataVector->R);

				//set max width and height
				int32 TextureWidth = MipMap->SizeX;
				int32 TextureHeight = MipMap->SizeY;
				FColor PixelColor;
				UE_LOG(LogTemp, Warning, TEXT("(max X: %d, max Y: %d)"), TextureWidth, TextureHeight);

				std::vector<std::vector<uint32>> tempHeights;
				std::vector<uint32> t;
				for (int32 i = 0; i < TextureWidth; i++)
				{
					t.clear();
					for (int32 j = 0; j < TextureHeight; j++)
					{
						//for every pixel in the image, iterate through and get the RGBA
						PixelColor.R = (uint8)ImageDataVector[j * TextureWidth + i].R;
						PixelColor.G = (uint8)ImageDataVector[j * TextureWidth + i].G;
						PixelColor.B = (uint8)ImageDataVector[j * TextureWidth + i].B;
						PixelColor.A = (uint8)ImageDataVector[j * TextureWidth + i].A;
						t.push_back(PixelColor.R + PixelColor.G + PixelColor.B);
						UE_LOG(LogTemp, Warning, TEXT("(%d, %d) | R:%u G:%u B:%u"), i, j, PixelColor.R, PixelColor.G, PixelColor.B);

						//set the pin height = to R+G+B
						float z = (float)PixelColor.R + (float)PixelColor.G + (float)PixelColor.B;
						FVector loc;

						//set the pin location = to the pixel's position in the image
						//and its z to displace by half its height (so its bottom is at z = 0)
						loc.X = float(i) * 100;
						loc.Y = float(j) * 100;
						loc.Z = z/2;
						UE_LOG(LogTemp, Warning, TEXT("z = %f"), z);
						SpawnActor(loc, FVector(1., 1., (z/100)));
					}
					tempHeights.push_back(t);
					
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

void FEditorWindowModule::SpawnActor(FVector SpawnLocation, FVector scale)
{
	FActorSpawnParameters SpawnParams;

	//Set a rotation of zero
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	//Spawn the actor - using BeginSpawningActorFromClass instead of NewObject
	//because we need to pass in the pin's scale before the constructor call ends
	FTransform SpawnTM(SpawnRotation, SpawnLocation, scale);
	AMyActor *pin = Cast<AMyActor>(UGameplayStatics::BeginSpawningActorFromClass(GWorld, AMyActor::StaticClass(), SpawnTM));
	if (pin)
	{
		UGameplayStatics::FinishSpawningActor(pin, SpawnTM);
	}
	pin->Spawn(SpawnLocation, SpawnRotation);

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