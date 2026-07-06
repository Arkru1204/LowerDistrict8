// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LD8CharacterBase.h"

#include "LD8PlayerCharacter.generated.h"

UCLASS()
class LOWERDISTRICT8_API ALD8PlayerCharacter : public ALD8CharacterBase
{
	GENERATED_BODY()


	/* ==================== Lifecycle ==================== */
public:
	// Sets default values for this character's properties
	ALD8PlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/* ==================== Getters ==================== */
public:
	/* Returns CameraBoom subobject */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/* Returns FollowCamera subobject */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
public:
	/* 현재 크로스헤어 벌어짐 정도 */
	UFUNCTION(BlueprintPure, Category = "Crosshair")
	float GetCurrentCrosshairSpread() const;


	/* ==================== Components ==================== */
private:
	/* 입력 처리용 컨트롤러 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController;

	/* 카메라-캐릭터 간 거리 유지, 카메라 회전에 사용 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* 실제 카메라 컴포넌트 (CameraBoom의 자식) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* 스킬 처리 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class ULD8SkillComponent* SkillComponent;


	/* ==================== Properties | Input Action ==================== */
private:
	/* 이동 InputAction */
	UPROPERTY(EditAnywhere, Category = "Properties|Input Action")
	class UInputAction* MoveAction;

	/* 카메라 이동, 회전 InputAction */
	UPROPERTY(EditAnywhere, Category = "Properties|Input Action")
	class UInputAction* LookAction;

	/* 점프 InputAction */
	UPROPERTY(EditAnywhere, Category = "Properties|Input Action")
	class UInputAction* JumpAction;

	/* 구르기 InputAction */
	UPROPERTY(EditAnywhere, Category = "Properties|Input Action")
	class UInputAction* RollAction;

	/* 카메라 시점 변경 InputAction */
	UPROPERTY(EditAnywhere, Category = "Properties|Input Action")
	class UInputAction* ChangeViewAction;

	/* 발사 InputAction */
	UPROPERTY(EditAnywhere, Category = "Properties|Input Action")
	class UInputAction* ShootAction;

	/* 스킬 InputAction */
	UPROPERTY(EditAnywhere, Category = "Properties|Input Action")
	class UInputAction* SkillAction;


	/* ==================== Properties | Recoil & Crosshair ==================== */

	/* 발사 시 카메라 쉐이크 */
	UPROPERTY(EditAnywhere, Category = "Properties|Recoil")
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	/* 카메라 쉐이크 강도 */
	UPROPERTY(EditAnywhere, Category = "Properties|Recoil")
	float FireCameraShakeScale = 0.5f;

	/* 기본 크로스헤어 벌어짐 */
	UPROPERTY(EditAnywhere, Category = "Properties|Crosshair")
	float MinCrosshairSpread = 0.4f;

	/* 최대 크로스헤어 벌어짐 */
	UPROPERTY(EditAnywhere, Category = "Properties|Crosshair")
	float MaxCrosshairSpread = 5.0f;

	/* 이동 중 추가되는 크로스헤어 벌어짐 */
	UPROPERTY(EditAnywhere, Category = "Properties|Crosshair")
	float MovementCrosshairSpread = 1.5f;

	/* 한 발 발사할 때 증가하는 크로스헤어 벌어짐 */
	UPROPERTY(EditAnywhere, Category = "Properties|Crosshair")
	float ShootCrosshairSpreadAmount = 1.0f;

	/* 크로스헤어가 다시 줄어드는 속도 */
	UPROPERTY(EditAnywhere, Category = "Properties|Crosshair")
	float CrosshairRecoverSpeed = 5.0f;

	/* 현재 크로스헤어 벌어짐 정도 */
	UPROPERTY(VisibleAnywhere, Category = "Properties|Crosshair")
	float CurrentCrosshairSpread = 0.4f;

	/* 구르기 중 카메라 높이 보정 배율 */
	UPROPERTY(EditAnywhere, Category = "Properties|Roll Camera")
	float RollCameraHeightCompensationScale = 1.0f;


	/* ==================== Input ==================== */
private:
	/* Move InputAction이 감지 될 때 호출되는 함수 */
	void MoveInput(const struct FInputActionValue& Value);
	/* Move InputAction이 끝났을 때 호출되는 함수 */
	void MoveInputCompleted(const struct FInputActionValue& Value);
	/* Look InputAction이 감지 될 때 호출되는 함수 */
	void LookInput(const struct FInputActionValue& Value);
	/* Jump InputAction이 감지 될 때 호출되는 함수 */
	void JumpInput();
	/* Roll InputAction이 감지 될 때 호출되는 함수 */
	void RollInput();
	/* ChangeView InputAction이 감지 될 때 호출되는 함수 */
	void ChangeView();
	/* Shoot InputAction이 감지 될 때 호출되는 함수 */
	void ShootInput();
	/* Skill InputAction이 감지 될 때 호출되는 함수 */
	void Skill();


	/* ==================== Runtime ==================== */
private:
	/* 시점 변경 처리 변수 */
	bool bIsLeftView = true;

private:
	/* 발사 성공 시 플레이어 피드백 처리 */
	void OnShotFired();

	/* 카메라 쉐이크 재생 */
	void PlayFireCameraShake();

	/* 크로스헤어 업데이트 */
	void UpdateCrosshairSpread(float DeltaTime);
	/* 크로스헤어 벌어짐 증가 */
	void AddShootCrosshairSpread();

private:
	/* 기본 카메라 붐 상대 위치 */
	FVector DefaultCameraBoomRelativeLocation = FVector::ZeroVector;
protected:
	/* 구르기 시작 시 호출 */
	virtual void OnRollStarted(float CapsuleHeightDelta) override;
	/* 구르기 종료 시 호출 */
	virtual void OnRollEnded() override;
};
