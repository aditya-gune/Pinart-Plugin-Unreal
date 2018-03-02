// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorWindowStyle.h"

class FEditorWindowCommands : public TCommands<FEditorWindowCommands>
{
public:

	FEditorWindowCommands()
		: TCommands<FEditorWindowCommands>(TEXT("Image Heightmap"), NSLOCTEXT("Contexts", "Image Heightmap", "EditorWindow Plugin"), NAME_None, FEditorWindowStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};