// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameOfLifeActor.generated.h"

USTRUCT(BlueprintType)
struct FIntVector2d
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y;

	FIntVector2d()
	{
		X = 0;
		Y = 0;
	}
	FIntVector2d(const int32 InX, const int32 InY)
	{
		X = InX;
		Y = InY;
	}

	/**
	* Compares this vector against another for equality.
	*
	* @param V The vector to compare against.
	* @return true if the two vectors are equal, otherwise false.
	*/
	bool operator==(const FIntVector2d& V) const
	{
		return X==V.X && Y==V.Y;
	}

	/**
	* Compares this vector against another for inequality.
	*
	* @param V The vector to compare against.
	* @return true if the two vectors are not equal, otherwise false.
	*/
	bool operator!=(const FIntVector2d& V) const
	{
		return X!=V.X || Y!=V.Y;
	}

};

FORCEINLINE uint32 GetTypeHash(const FIntVector2d& Vector)
{
	return FCrc::MemCrc32(&Vector,sizeof(Vector));
}

UCLASS()
class CONVAYSLIFE_API AGameOfLifeActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere, meta=(AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* Grid;
	
public:	
	// Sets default values for this actor's properties
	AGameOfLifeActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2d GameDimensions = FIntVector2d(10, 10);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<FIntVector2d> CurrentState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<FIntVector2d> AllCells;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Step = 125.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GameTickRate = 0.1f;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle GameTimer;
	UFUNCTION(BlueprintCallable)
	void CalcNextState();
	UFUNCTION(BlueprintCallable)
	void UpdateStateVisuals(TSet<FIntVector2d> CellsToUpdate);
	UFUNCTION(BlueprintCallable)
	TSet<FIntVector2d> FindPossiblyChanged() const;
	UFUNCTION(BlueprintCallable)
	bool WillLive(FIntVector2d Cell) const;
	UFUNCTION(BlueprintCallable)
	bool IsValidCell(FIntVector2d Cell) const;
	UFUNCTION(BlueprintCallable)
	bool IsAliveCell(FIntVector2d Cell) const;
	UFUNCTION(BlueprintCallable)
	TSet<FIntVector2d> GetAllNearbyCells(FIntVector2d Cell) const;
	UFUNCTION(BlueprintCallable)
	int32 GetLiveNeighbors(FIntVector2d Cell) const;
	UFUNCTION(BlueprintCallable)
	void InitAllCells();
	UFUNCTION(BlueprintCallable)
	TArray<FTransform> CreateCellTransformArray(TSet<FIntVector2d> Cells);
	UFUNCTION(BlueprintCallable)
	int32 GetCellInstanceID(FIntVector2d Cell) const;
	UFUNCTION(BlueprintCallable)
	FIntVector2d GetCellFromInstanceID(int32 ID) const;
	UFUNCTION(BlueprintCallable)
	bool IsStaleState(TSet<FIntVector2d> State) const;
	UFUNCTION(BlueprintCallable)
	bool IsGameActive() const;
	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void StopGame();
	UFUNCTION(BlueprintCallable)
	void ToggleCell(FIntVector2d Cell, bool Active);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
