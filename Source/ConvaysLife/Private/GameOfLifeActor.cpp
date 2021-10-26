// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOfLifeActor.h"



// Sets default values
AGameOfLifeActor::AGameOfLifeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Grid = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Grid"));
	Grid->NumCustomDataFloats = 1;
	RootComponent = Grid;
}

void AGameOfLifeActor::CalcNextState()
{
	TSet<FIntVector2d> PossiblyChanged = FindPossiblyChanged();
	TSet<FIntVector2d> NextState;
	for (auto Cell : PossiblyChanged)
	{
		if (WillLive(Cell))
		{
			NextState.Add(Cell);
		}
	}
	if (IsStaleState(NextState))
	{
		StopGame();
	}
	CurrentState = NextState;
	UpdateStateVisuals(PossiblyChanged);
}

void AGameOfLifeActor::UpdateStateVisuals(TSet<FIntVector2d> CellsToUpdate)
{
	if (!Grid) return;

	for (auto Cell : CellsToUpdate)
	{
		int32 ID = GetCellInstanceID(Cell);
		Grid->SetCustomDataValue(ID, 0, IsAliveCell(Cell) ? 1.f : 0.f, true);
	}
}

TSet<FIntVector2d> AGameOfLifeActor::FindPossiblyChanged() const
{
	TSet<FIntVector2d> PossiblyChanged = CurrentState;
	TSet<FIntVector2d> Iterator = PossiblyChanged;
	for (auto Cell : Iterator)
	{
		for (auto NearbyCell : GetAllNearbyCells(Cell))
		{
			PossiblyChanged.Add(NearbyCell);
		}
	}
	return PossiblyChanged;
}

bool AGameOfLifeActor::WillLive(FIntVector2d Cell) const
{
	if (!IsValidCell(Cell)) return false;
	int32 Neighbors = GetLiveNeighbors(Cell);
	if (IsAliveCell(Cell))
	{
		return Neighbors == 2 || Neighbors == 3;
	}
	return Neighbors == 3;

}

bool AGameOfLifeActor::IsValidCell(FIntVector2d Cell) const
{
	return (Cell.X >= 0 && Cell.X < GameDimensions.X) && (Cell.Y >= 0 && Cell.Y < GameDimensions.Y);
}

bool AGameOfLifeActor::IsAliveCell(FIntVector2d Cell) const
{
	return CurrentState.Contains(Cell);
}

TSet<FIntVector2d> AGameOfLifeActor::GetAllNearbyCells(FIntVector2d Cell) const
{
	if (!IsValidCell(Cell)) return {};

	TSet<FIntVector2d> NearbyCells;
	TSet<FIntVector2d> Iterator;
	Iterator.Add(FIntVector2d(Cell.X, Cell.Y + 1));
	Iterator.Add(FIntVector2d(Cell.X, Cell.Y - 1));
	Iterator.Add(FIntVector2d(Cell.X + 1, Cell.Y + 1));
	Iterator.Add(FIntVector2d(Cell.X - 1, Cell.Y - 1));
	Iterator.Add(FIntVector2d(Cell.X - 1, Cell.Y + 1));
	Iterator.Add(FIntVector2d(Cell.X + 1, Cell.Y - 1));
	Iterator.Add(FIntVector2d(Cell.X + 1, Cell.Y));
	Iterator.Add(FIntVector2d(Cell.X - 1, Cell.Y));
	for (auto IterCell : Iterator)
	{
		if (IsValidCell(IterCell))
		{
			NearbyCells.Add(IterCell);
		}
	}
	return NearbyCells;
}

int32 AGameOfLifeActor::GetLiveNeighbors(FIntVector2d Cell) const
{
	int32 Counter = 0;
	for (auto IterCell : GetAllNearbyCells(Cell))
	{
		if (IsAliveCell(IterCell))
		{
			Counter++;
		}
	}
	return Counter;
}

void AGameOfLifeActor::InitAllCells()
{
	AllCells.Empty(GameDimensions.X * GameDimensions.Y);
	for (uint8 a = 0; a < GameDimensions.X; a++)
	{
		for (uint8 b = 0; b < GameDimensions.Y; b++)
		{
			AllCells.Add(FIntVector2d(a, b));
		}
	}
	if (!Grid) return;
	Grid->ClearInstances();
	Grid->AddInstances(CreateCellTransformArray(AllCells), false);
	UpdateStateVisuals(AllCells);
}

TArray<FTransform> AGameOfLifeActor::CreateCellTransformArray(TSet<FIntVector2d> Cells)
{
	TArray<FTransform> TempTransforms;
	for (auto Cell : Cells)
	{
		TempTransforms.Add(FTransform(FVector(Cell.X * Step, Cell.Y * Step, 0)));
	}
	return TempTransforms;
}

int32 AGameOfLifeActor::GetCellInstanceID(FIntVector2d Cell) const
{
	return Cell.X * GameDimensions.Y + Cell.Y;
}

FIntVector2d AGameOfLifeActor::GetCellFromInstanceID(int32 ID) const
{
	FIntVector2d Cell;
	Cell.X = ID / GameDimensions.X;
	Cell.Y = ID % GameDimensions.X;
	return Cell;
}

bool AGameOfLifeActor::IsStaleState(TSet<FIntVector2d> State) const
{
	//should be true if something has changed between the two
	return State.Difference(CurrentState).Num() == 0 && CurrentState.Difference(State).Num() == 0;
}

bool AGameOfLifeActor::IsGameActive() const
{
	return GetWorldTimerManager().IsTimerActive(GameTimer);
}

void AGameOfLifeActor::StartGame()
{
	UpdateStateVisuals(AllCells);
	FTimerDelegate Del;
	Del.BindUObject(this, &AGameOfLifeActor::CalcNextState);
	GetWorldTimerManager().SetTimer(GameTimer, Del, GameTickRate, true);
}

void AGameOfLifeActor::StopGame()
{
	GetWorldTimerManager().ClearTimer(GameTimer);
}

void AGameOfLifeActor::ToggleCell(FIntVector2d Cell, bool Active)
{
	if (Active)
	{
		CurrentState.Add(Cell);
	}
	else
	{
		CurrentState.Remove(Cell);
	}
	TSet<FIntVector2d>Set = {Cell};
	UpdateStateVisuals(Set);
}

// Called when the game starts or when spawned
void AGameOfLifeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameOfLifeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

