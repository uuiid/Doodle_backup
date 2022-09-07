#include "DoodleAnimInstance.h"

UDoodleAnimInstance::UDoodleAnimInstance()
    : Super()
{
}

void UDoodleAnimInstance::DoodleCalculateSpeed()
{

    APawn *LPawn = TryGetPawnOwner();

    if (LPawn)
    {
        FVector LVelocity = LPawn->GetVelocity();
        DirectionAttrXY = CalculateDirection(
            LVelocity,
            LPawn->GetBaseAimRotation());
    }
}

void UDoodleAnimInstance::DoodleLookAtObject(const AActor *InActor)
{
    // const APawn *LInPawn = Cast<const APawn>(InActor);
    APawn *LSelfPawn = TryGetPawnOwner();
    // AActor *LSelfPawn = GetOwningActor();
    if (LSelfPawn)
    {
        FVector LInVector = InActor->GetActorLocation();
        FVector LSelfVector = LSelfPawn->GetActorLocation();

        FVector LVector = LInVector - LSelfVector;
        // LVector.Normalize();
        // FRotator LRot = LVector.Rotation();
        // DirectionAttrXY = LRot.Yaw;
        // DirectionAttrZ = LRot.Pitch;
        // UE_LOG(LogTemp, Log, TEXT("LRot: %s"), *(LRot.ToString()));
        if (!LVector.IsNearlyZero())
        {
            // 此处代码是根据 CalculateDirection 函数来的
            FMatrix LSelfRot = FRotationMatrix{
                LSelfPawn->GetBaseAimRotation()};

            FVector LXYNormal = LVector.GetSafeNormal2D();
            FVector LForward = LSelfRot.GetScaledAxis(EAxis::X);
            // UE_LOG(LogTemp, Log, TEXT("LForward: %s"), *(LForward.ToString()));

            // 获取向前矢量和速度(XY平面) 的cos -> 弧度值
            float ForwardCosAngle = FVector::DotProduct(LForward, LXYNormal);
            // 将弧度值转换为度
            DirectionAttrXY = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

            // 判断向前轴和速度方向, 根据结果进行翻转
            float RightCosAngle = FVector::DotProduct(LSelfRot.GetScaledAxis(EAxis::Y), LXYNormal);
            if (RightCosAngle < 0)
            {
                DirectionAttrXY *= -1;
            }

            // 这下面是上下角度的代码
            // FVector2D LZNormal2D{LVector.X, LVector.Z};
            // LZNormal2D.Normalize();
            // FVector LXZNormal{LZNormal2D.X, 0.0f, LZNormal2D.Y};
            FVector LXZNormal = FVector::VectorPlaneProject(LVector, LSelfRot.GetScaledAxis(EAxis::Y));
            LXZNormal.Normalize();
            // UE_LOG(LogTemp, Log, TEXT("LXZNormal: %s"), *(LXZNormal.ToString()));

            // 获取向前矢量和速度(XZ平面) 的cos -> 弧度值
            ForwardCosAngle = FVector::DotProduct(LForward, LXZNormal);
            // 将弧度值转换为度
            DirectionAttrZ = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

            // 判断向前轴和速度方向, 根据结果进行翻转
            RightCosAngle = FVector::DotProduct(LSelfRot.GetScaledAxis(EAxis::Z), LXZNormal);
            if (RightCosAngle < 0)
            {
                DirectionAttrZ *= -1;
            }
            // UE_LOG(LogTemp, Log, TEXT("DirectionAttrXY %f DirectionAttrZ: %f"), DirectionAttrXY, DirectionAttrZ);
        }
    }
}

void UDoodleAnimInstance::DoodleRandom()
{
    // 获得物体创建时间
    float time = GetOwningActor()->GetGameTimeSinceCreation() / 2.0f + RandomAttr_InstallValue;

    // 生成噪波函数
    float noise = FMath::PerlinNoise1D(time);
    noise = FMath::Abs(noise);
    // FMath::DivideAndRoundNearest()

    // 获得最大和最小猪
    float tmp_Clamp = FMath::Clamp(noise, 0.25f, 0.75f);
    RandomAttr = FMath::GetMappedRangeValueClamped({0.25f, 0.75f}, {0.0f, 1.0f}, noise);
    UE_LOG(LogTemp, Log, TEXT("RandomAttr %f "), RandomAttr);
}

void UDoodleAnimInstance::NativeBeginPlay()
{
    UAnimInstance::NativeBeginPlay();
    RandomAttr_InstallValue = FMath::RandRange(0.0, 256.0f);
}