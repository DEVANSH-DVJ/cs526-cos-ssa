; ModuleID = 'multiple_partitions.c'
source_filename = "multiple_partitions.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@i = dso_local global i32 0, align 4
@USEVAR = external global i32, align 4
@j = dso_local global i32 0, align 4
@INPUT1 = external global i32, align 4
@INPUT2 = external global i32, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @fn1() #0 {
  %1 = load i32, ptr @i, align 4
  %2 = add nsw i32 %1, 20
  store i32 %2, ptr @i, align 4
  %3 = load i32, ptr @i, align 4
  store i32 %3, ptr @USEVAR, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @fn2() #0 {
  %1 = load i32, ptr @i, align 4
  %2 = add nsw i32 %1, 20
  store i32 %2, ptr @i, align 4
  %3 = load i32, ptr @i, align 4
  store i32 %3, ptr @USEVAR, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr @i, align 4
  store i32 0, ptr @j, align 4
  %2 = load i32, ptr @INPUT1, align 4
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %0
  store i32 0, ptr @i, align 4
  call void @fn1()
  call void @fn2()
  br label %6

5:                                                ; preds = %0
  store i32 10, ptr @i, align 4
  call void @fn1()
  call void @fn2()
  br label %6

6:                                                ; preds = %5, %4
  %7 = load i32, ptr @INPUT2, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %6
  store i32 30, ptr @j, align 4
  call void @fn1()
  call void @fn2()
  br label %11

10:                                               ; preds = %6
  store i32 1, ptr @j, align 4
  call void @fn1()
  call void @fn2()
  br label %11

11:                                               ; preds = %10, %9
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.7"}
