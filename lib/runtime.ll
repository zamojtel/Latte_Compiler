; ModuleID = 'functions.cpp'
source_filename = "functions.cpp"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, %struct._IO_codecvt*, %struct._IO_wide_data*, %struct._IO_FILE*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type opaque
%struct._IO_codecvt = type opaque
%struct._IO_wide_data = type opaque

@string_count = dso_local global i32 0, align 4
@.str = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@.str.2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.3 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@stdin = external global %struct._IO_FILE*, align 8

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i32 @getArrayLength(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i32*, align 8
  store i8* %0, i8** %2, align 8
  %4 = load i8*, i8** %2, align 8
  %5 = bitcast i8* %4 to i32*
  store i32* %5, i32** %3, align 8
  %6 = load i32*, i32** %3, align 8
  %7 = getelementptr inbounds i32, i32* %6, i32 -1
  store i32* %7, i32** %3, align 8
  %8 = load i32*, i32** %3, align 8
  %9 = load i32, i32* %8, align 4
  ret i32 %9
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i8* @allocArray(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32*, align 8
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %6 = load i32, i32* %4, align 4
  %7 = load i32, i32* %3, align 4
  %8 = mul nsw i32 %6, %7
  %9 = sext i32 %8 to i64
  %10 = add i64 %9, 4
  %11 = call noalias i8* @malloc(i64 noundef %10) #7
  %12 = bitcast i8* %11 to i32*
  store i32* %12, i32** %5, align 8
  %13 = load i32, i32* %3, align 4
  %14 = load i32*, i32** %5, align 8
  %15 = getelementptr inbounds i32, i32* %14, i64 0
  store i32 %13, i32* %15, align 4
  %16 = load i32*, i32** %5, align 8
  %17 = getelementptr inbounds i32, i32* %16, i64 1
  %18 = bitcast i32* %17 to i8*
  %19 = load i32, i32* %3, align 4
  %20 = load i32, i32* %4, align 4
  %21 = mul nsw i32 %19, %20
  %22 = sext i32 %21 to i64
  call void @llvm.memset.p0i8.i64(i8* align 4 %18, i8 0, i64 %22, i1 false)
  %23 = load i32*, i32** %5, align 8
  %24 = getelementptr inbounds i32, i32* %23, i64 1
  %25 = bitcast i32* %24 to i8*
  ret i8* %25
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64 noundef) #1

; Function Attrs: argmemonly nofree nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i32 @getStringCount() #0 {
  %1 = load i32, i32* @string_count, align 4
  ret i32 %1
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @error() #0 {
  ret void
}

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @printString(i8* noundef %0) #3 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %4 = load i8*, i8** %2, align 8
  %5 = icmp eq i8* %4, null
  br i1 %5, label %6, label %8

6:                                                ; preds = %1
  %7 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0))
  br label %13

8:                                                ; preds = %1
  %9 = load i8*, i8** %2, align 8
  store i8* %9, i8** %3, align 8
  %10 = load i8*, i8** %3, align 8
  %11 = getelementptr inbounds i8, i8* %10, i64 5
  %12 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i64 0, i64 0), i8* noundef %11)
  br label %13

13:                                               ; preds = %8, %6
  ret void
}

declare i32 @printf(i8* noundef, ...) #4

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @printInt(i32 noundef %0) #3 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.2, i64 0, i64 0), i32 noundef %3)
  ret void
}

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local i32 @readInt() #3 {
  %1 = alloca i32, align 4
  %2 = alloca i8, align 1
  %3 = call i32 (i8*, ...) @__isoc99_scanf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.3, i64 0, i64 0), i32* noundef %1)
  br label %4

4:                                                ; preds = %14, %0
  %5 = call i32 @getchar()
  %6 = trunc i32 %5 to i8
  store i8 %6, i8* %2, align 1
  %7 = sext i8 %6 to i32
  %8 = icmp ne i32 %7, -1
  br i1 %8, label %9, label %15

9:                                                ; preds = %4
  %10 = load i8, i8* %2, align 1
  %11 = sext i8 %10 to i32
  %12 = icmp eq i32 %11, 10
  br i1 %12, label %13, label %14

13:                                               ; preds = %9
  br label %15

14:                                               ; preds = %9
  br label %4, !llvm.loop !6

15:                                               ; preds = %13, %4
  %16 = load i32, i32* %1, align 4
  ret i32 %16
}

declare i32 @__isoc99_scanf(i8* noundef, ...) #4

declare i32 @getchar() #4

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @increaseStringCounter(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %4 = load i8*, i8** %2, align 8
  store i8* %4, i8** %3, align 8
  %5 = load i8*, i8** %3, align 8
  %6 = getelementptr inbounds i8, i8* %5, i64 0
  %7 = load i8, i8* %6, align 1
  %8 = icmp ne i8 %7, 0
  br i1 %8, label %9, label %15

9:                                                ; preds = %1
  %10 = load i8*, i8** %3, align 8
  %11 = getelementptr inbounds i8, i8* %10, i64 1
  %12 = bitcast i8* %11 to i32*
  %13 = load i32, i32* %12, align 4
  %14 = add nsw i32 %13, 1
  store i32 %14, i32* %12, align 4
  br label %15

15:                                               ; preds = %9, %1
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @decreaseStringCounter(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %4 = load i8*, i8** %2, align 8
  store i8* %4, i8** %3, align 8
  %5 = load i8*, i8** %3, align 8
  %6 = getelementptr inbounds i8, i8* %5, i64 0
  %7 = load i8, i8* %6, align 1
  %8 = icmp ne i8 %7, 0
  br i1 %8, label %9, label %25

9:                                                ; preds = %1
  %10 = load i8*, i8** %3, align 8
  %11 = getelementptr inbounds i8, i8* %10, i64 1
  %12 = bitcast i8* %11 to i32*
  %13 = load i32, i32* %12, align 4
  %14 = add nsw i32 %13, -1
  store i32 %14, i32* %12, align 4
  %15 = load i8*, i8** %3, align 8
  %16 = getelementptr inbounds i8, i8* %15, i64 1
  %17 = bitcast i8* %16 to i32*
  %18 = load i32, i32* %17, align 4
  %19 = icmp eq i32 %18, 0
  br i1 %19, label %20, label %24

20:                                               ; preds = %9
  %21 = load i32, i32* @string_count, align 4
  %22 = add nsw i32 %21, -1
  store i32 %22, i32* @string_count, align 4
  %23 = load i8*, i8** %2, align 8
  call void @free(i8* noundef %23) #7
  br label %24

24:                                               ; preds = %20, %9
  br label %25

25:                                               ; preds = %24, %1
  ret void
}

; Function Attrs: nounwind
declare void @free(i8* noundef) #1

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local i8* @readString() #3 {
  %1 = alloca i8*, align 8
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = alloca i8*, align 8
  store i8* null, i8** %1, align 8
  %5 = load %struct._IO_FILE*, %struct._IO_FILE** @stdin, align 8
  %6 = call i64 @getline(i8** noundef %1, i64* noundef %2, %struct._IO_FILE* noundef %5)
  store i64 %6, i64* %3, align 8
  %7 = load i64, i64* %3, align 8
  %8 = icmp ugt i64 %7, 0
  br i1 %8, label %9, label %22

9:                                                ; preds = %0
  %10 = load i8*, i8** %1, align 8
  %11 = load i64, i64* %3, align 8
  %12 = sub i64 %11, 1
  %13 = getelementptr inbounds i8, i8* %10, i64 %12
  %14 = load i8, i8* %13, align 1
  %15 = sext i8 %14 to i32
  %16 = icmp eq i32 %15, 10
  br i1 %16, label %17, label %22

17:                                               ; preds = %9
  %18 = load i8*, i8** %1, align 8
  %19 = load i64, i64* %3, align 8
  %20 = sub i64 %19, 1
  %21 = getelementptr inbounds i8, i8* %18, i64 %20
  store i8 0, i8* %21, align 1
  br label %22

22:                                               ; preds = %17, %9, %0
  %23 = load i64, i64* %2, align 8
  %24 = trunc i64 %23 to i32
  %25 = call i8* @allocString(i32 noundef %24)
  store i8* %25, i8** %4, align 8
  %26 = load i8*, i8** %4, align 8
  %27 = getelementptr inbounds i8, i8* %26, i64 4
  %28 = getelementptr inbounds i8, i8* %27, i64 1
  %29 = load i8*, i8** %1, align 8
  %30 = load i64, i64* %2, align 8
  %31 = add i64 %30, 1
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %28, i8* align 1 %29, i64 %31, i1 false)
  %32 = load i8*, i8** %4, align 8
  ret i8* %32
}

declare i64 @getline(i8** noundef, i64* noundef, %struct._IO_FILE* noundef) #4

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i8* @allocString(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i8*, align 8
  store i32 %0, i32* %2, align 4
  %4 = load i32, i32* %2, align 4
  %5 = add nsw i32 4, %4
  %6 = add nsw i32 %5, 1
  %7 = add nsw i32 %6, 1
  %8 = sext i32 %7 to i64
  %9 = mul i64 1, %8
  %10 = call noalias i8* @malloc(i64 noundef %9) #7
  store i8* %10, i8** %3, align 8
  %11 = load i8*, i8** %3, align 8
  %12 = getelementptr inbounds i8, i8* %11, i64 0
  store i8 1, i8* %12, align 1
  %13 = load i8*, i8** %3, align 8
  %14 = getelementptr inbounds i8, i8* %13, i64 1
  %15 = bitcast i8* %14 to i32*
  store i32 0, i32* %15, align 4
  %16 = load i32, i32* @string_count, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, i32* @string_count, align 4
  %18 = load i8*, i8** %3, align 8
  ret i8* %18
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #5

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local i8* @addStrings(i8* noundef %0, i8* noundef %1) #3 {
  %3 = alloca i8*, align 8
  %4 = alloca i8*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i8*, align 8
  store i8* %0, i8** %3, align 8
  store i8* %1, i8** %4, align 8
  %8 = load i8*, i8** %3, align 8
  %9 = icmp ne i8* %8, null
  br i1 %9, label %10, label %15

10:                                               ; preds = %2
  %11 = load i8*, i8** %3, align 8
  %12 = getelementptr inbounds i8, i8* %11, i64 4
  %13 = getelementptr inbounds i8, i8* %12, i64 1
  %14 = call i64 @strlen(i8* noundef %13) #8
  br label %16

15:                                               ; preds = %2
  br label %16

16:                                               ; preds = %15, %10
  %17 = phi i64 [ %14, %10 ], [ 0, %15 ]
  %18 = trunc i64 %17 to i32
  store i32 %18, i32* %5, align 4
  %19 = load i8*, i8** %4, align 8
  %20 = icmp ne i8* %19, null
  br i1 %20, label %21, label %26

21:                                               ; preds = %16
  %22 = load i8*, i8** %4, align 8
  %23 = getelementptr inbounds i8, i8* %22, i64 4
  %24 = getelementptr inbounds i8, i8* %23, i64 1
  %25 = call i64 @strlen(i8* noundef %24) #8
  br label %27

26:                                               ; preds = %16
  br label %27

27:                                               ; preds = %26, %21
  %28 = phi i64 [ %25, %21 ], [ 0, %26 ]
  %29 = trunc i64 %28 to i32
  store i32 %29, i32* %6, align 4
  %30 = load i32, i32* %5, align 4
  %31 = load i32, i32* %6, align 4
  %32 = add nsw i32 %30, %31
  %33 = call i8* @allocString(i32 noundef %32)
  store i8* %33, i8** %7, align 8
  %34 = load i32, i32* %5, align 4
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %36, label %45

36:                                               ; preds = %27
  %37 = load i8*, i8** %7, align 8
  %38 = getelementptr inbounds i8, i8* %37, i64 4
  %39 = getelementptr inbounds i8, i8* %38, i64 1
  %40 = load i8*, i8** %3, align 8
  %41 = getelementptr inbounds i8, i8* %40, i64 4
  %42 = getelementptr inbounds i8, i8* %41, i64 1
  %43 = load i32, i32* %5, align 4
  %44 = sext i32 %43 to i64
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %39, i8* align 1 %42, i64 %44, i1 false)
  br label %45

45:                                               ; preds = %36, %27
  %46 = load i32, i32* %6, align 4
  %47 = icmp ne i32 %46, 0
  br i1 %47, label %48, label %60

48:                                               ; preds = %45
  %49 = load i8*, i8** %7, align 8
  %50 = getelementptr inbounds i8, i8* %49, i64 4
  %51 = getelementptr inbounds i8, i8* %50, i64 1
  %52 = load i32, i32* %5, align 4
  %53 = sext i32 %52 to i64
  %54 = getelementptr inbounds i8, i8* %51, i64 %53
  %55 = load i8*, i8** %4, align 8
  %56 = getelementptr inbounds i8, i8* %55, i64 4
  %57 = getelementptr inbounds i8, i8* %56, i64 1
  %58 = load i32, i32* %6, align 4
  %59 = sext i32 %58 to i64
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %54, i8* align 1 %57, i64 %59, i1 false)
  br label %60

60:                                               ; preds = %48, %45
  %61 = load i8*, i8** %7, align 8
  %62 = load i32, i32* %5, align 4
  %63 = sext i32 %62 to i64
  %64 = getelementptr inbounds i8, i8* %61, i64 %63
  %65 = load i32, i32* %6, align 4
  %66 = sext i32 %65 to i64
  %67 = getelementptr inbounds i8, i8* %64, i64 %66
  %68 = getelementptr inbounds i8, i8* %67, i64 4
  %69 = getelementptr inbounds i8, i8* %68, i64 1
  store i8 0, i8* %69, align 1
  %70 = load i8*, i8** %7, align 8
  ret i8* %70
}

; Function Attrs: nounwind readonly willreturn
declare i64 @strlen(i8* noundef) #6

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local zeroext i1 @compareStrings(i8* noundef %0, i8* noundef %1) #0 {
  %3 = alloca i1, align 1
  %4 = alloca i8*, align 8
  %5 = alloca i8*, align 8
  store i8* %0, i8** %4, align 8
  store i8* %1, i8** %5, align 8
  %6 = load i8*, i8** %4, align 8
  %7 = load i8*, i8** %5, align 8
  %8 = icmp eq i8* %6, %7
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  store i1 true, i1* %3, align 1
  br label %26

10:                                               ; preds = %2
  %11 = load i8*, i8** %4, align 8
  %12 = icmp eq i8* %11, null
  br i1 %12, label %16, label %13

13:                                               ; preds = %10
  %14 = load i8*, i8** %5, align 8
  %15 = icmp eq i8* %14, null
  br i1 %15, label %16, label %17

16:                                               ; preds = %13, %10
  store i1 false, i1* %3, align 1
  br label %26

17:                                               ; preds = %13
  %18 = load i8*, i8** %4, align 8
  %19 = getelementptr inbounds i8, i8* %18, i64 4
  %20 = getelementptr inbounds i8, i8* %19, i64 1
  %21 = load i8*, i8** %5, align 8
  %22 = getelementptr inbounds i8, i8* %21, i64 4
  %23 = getelementptr inbounds i8, i8* %22, i64 1
  %24 = call i32 @strcmp(i8* noundef %20, i8* noundef %23) #8
  %25 = icmp eq i32 %24, 0
  store i1 %25, i1* %3, align 1
  br label %26

26:                                               ; preds = %17, %16, %9
  %27 = load i1, i1* %3, align 1
  ret i1 %27
}

; Function Attrs: nounwind readonly willreturn
declare i32 @strcmp(i8* noundef, i8* noundef) #6

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local zeroext i1 @isConstant(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca i8, align 1
  store i8* %0, i8** %2, align 8
  %5 = load i8*, i8** %2, align 8
  store i8* %5, i8** %3, align 8
  %6 = load i8*, i8** %3, align 8
  %7 = getelementptr inbounds i8, i8* %6, i64 0
  %8 = load i8, i8* %7, align 1
  store i8 %8, i8* %4, align 1
  %9 = load i8, i8* %4, align 1
  %10 = sext i8 %9 to i32
  %11 = icmp ne i32 %10, 0
  ret i1 %11
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i32 @getReferenceCount(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca i32*, align 8
  store i8* %0, i8** %2, align 8
  %5 = load i8*, i8** %2, align 8
  store i8* %5, i8** %3, align 8
  %6 = load i8*, i8** %3, align 8
  %7 = getelementptr inbounds i8, i8* %6, i64 1
  %8 = bitcast i8* %7 to i32*
  store i32* %8, i32** %4, align 8
  %9 = load i32*, i32** %4, align 8
  %10 = load i32, i32* %9, align 4
  ret i32 %10
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local void @freeString(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = load i8*, i8** %2, align 8
  call void @free(i8* noundef %3) #7
  ret void
}

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local i32 @myFunc(i32* noundef %0, i32 noundef %1) #3 {
  %3 = alloca i32*, align 8
  %4 = alloca i32, align 4
  store i32* %0, i32** %3, align 8
  store i32 %1, i32* %4, align 4
  %5 = load i32*, i32** %3, align 8
  %6 = getelementptr inbounds i32, i32* %5, i64 0
  %7 = load i32, i32* %6, align 4
  %8 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.3, i64 0, i64 0), i32 noundef %7)
  ret i32 1
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i8* @allocateInstance(i8** noundef %0, i32 noundef %1) #0 {
  %3 = alloca i8**, align 8
  %4 = alloca i32, align 4
  %5 = alloca i8*, align 8
  store i8** %0, i8*** %3, align 8
  store i32 %1, i32* %4, align 4
  %6 = load i32, i32* %4, align 4
  %7 = add nsw i32 8, %6
  %8 = sext i32 %7 to i64
  %9 = call noalias i8* @malloc(i64 noundef %8) #7
  store i8* %9, i8** %5, align 8
  %10 = load i8*, i8** %5, align 8
  %11 = getelementptr inbounds i8, i8* %10, i64 8
  %12 = load i32, i32* %4, align 4
  %13 = sext i32 %12 to i64
  call void @llvm.memset.p0i8.i64(i8* align 1 %11, i8 0, i64 %13, i1 false)
  %14 = load i8**, i8*** %3, align 8
  %15 = load i8*, i8** %5, align 8
  %16 = bitcast i8* %15 to i8***
  store i8** %14, i8*** %16, align 8
  %17 = load i8*, i8** %5, align 8
  %18 = getelementptr inbounds i8, i8* %17, i64 8
  ret i8* %18
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i8** @get_vtable(i8* noundef %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = load i8*, i8** %2, align 8
  %4 = getelementptr inbounds i8, i8* %3, i64 -8
  %5 = bitcast i8* %4 to i8***
  %6 = load i8**, i8*** %5, align 8
  ret i8** %6
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define dso_local i8* @getField(i8* noundef %0, i32 noundef %1) #0 {
  %3 = alloca i8*, align 8
  %4 = alloca i32, align 4
  store i8* %0, i8** %3, align 8
  store i32 %1, i32* %4, align 4
  %5 = load i8*, i8** %3, align 8
  %6 = load i32, i32* %4, align 4
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds i8, i8* %5, i64 %7
  ret i8* %8
}

attributes #0 = { mustprogress noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { argmemonly nofree nounwind willreturn writeonly }
attributes #3 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { argmemonly nofree nounwind willreturn }
attributes #6 = { nounwind readonly willreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind }
attributes #8 = { nounwind readonly willreturn }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Debian clang version 14.0.6"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}