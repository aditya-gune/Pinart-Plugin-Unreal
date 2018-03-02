// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "EditorWindowCommands.h"

#define LOCTEXT_NAMESPACE "FEditorWindowModule"

void FEditorWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Image Heightmap", "Bring up Image Heightmap window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
