define dso_local noundef i32** @makeArray(i32 noundef %0, i32 noundef %1) {
%3 = alloca i32, align 4
%4 = alloca i32, align 4
%5 = alloca i32**, align 8
%6 = alloca i32, align 4
%7 = alloca i32, align 4
store i32 %0, i32* %3, align 4
store i32 %1, i32* %4, align 4
%8 = load i32, i32* %3, align 4
%9 = call noundef i8* @allocArray(i32 noundef %8,i32 noundef 8)
%10 = bitcast i8* %9 to i32**
store i32** %10, i32*** %5, align 8
store i32 0, i32* %6, align 4
br label %Label0
Label0:
%11 = load i32, i32* %6, align 4
%12 = load i32, i32* %3, align 4
%13 = icmp slt i32 %11, %12
br i1 %13, label %Label5, label %Label3
Label5:
store i32 0, i32* %7, align 4
%14 = load i32**, i32*** %5, align 8
%15 = load i32, i32* %6, align 4
%16 = getelementptr inbounds i32*, i32** %14, i32 %15
%17 = load i32, i32* %4, align 4
%18 = call noundef i8* @allocArray(i32 noundef %17,i32 noundef 4)
%19 = bitcast i8* %18 to i32*
store i32* %19, i32** %16, align 8
br label %Label1
Label1:
%20 = load i32, i32* %7, align 4
%21 = load i32, i32* %4, align 4
%22 = icmp slt i32 %20, %21
br i1 %22, label %Label6, label %Label2
Label6:
%23 = load i32**, i32*** %5, align 8
%24 = load i32, i32* %6, align 4
%25 = getelementptr inbounds i32*, i32** %23, i32 %24
%26 = load i32*, i32** %25, align 8
%27 = load i32, i32* %7, align 4
%28 = getelementptr inbounds i32, i32* %26, i32 %27
%29 = load i32, i32* %6, align 4
%30 = add nsw i32 %29, 1
%31 = load i32, i32* %7, align 4
%32 = add nsw i32 %31, 1
%33 = mul nsw i32 %30, %32
store i32 %33, i32* %28, align 4
%34 = load i32, i32* %7, align 4
%35 = add nsw i32 %34, 1
store i32 %35, i32* %7, align 4
br label %Label1
Label2:
%36 = load i32, i32* %6, align 4
%37 = add nsw i32 %36, 1
store i32 %37, i32* %6, align 4
br label %Label0
Label3:
%38 = load i32**, i32*** %5, align 8
ret i32** %38
}
define dso_local void @printArray(i32** noundef %0) {
%2 = alloca i32**, align 8
%3 = alloca i32, align 4
%4 = alloca i32*, align 8
%5 = alloca i32, align 4
%6 = alloca i32, align 4
store i32** %0, i32*** %2, align 8
store i32 0, i32* %3, align 4
store i32* null, i32** %4, align 8
%7 = load i32**, i32*** %2, align 8
%8 = bitcast i32** %7 to i8*
%9 = call noundef i32 @getArrayLength(i8* noundef %8)
br label %Label0
Label0:
%10 = load i32, i32* %3, align 4
%11 = icmp slt i32 %10, %9
br i1 %11, label %Label5, label %Label3
Label5:
%12 = load i32**, i32*** %2, align 8
%13 = load i32, i32* %3, align 4
%14 = getelementptr inbounds i32*, i32** %12, i32 %13
%15 = load i32*, i32** %14, align 8
store i32* %15, i32** %4, align 8
store i32 0, i32* %5, align 4
store i32 0, i32* %6, align 4
%16 = load i32*, i32** %4, align 8
%17 = bitcast i32* %16 to i8*
%18 = call noundef i32 @getArrayLength(i8* noundef %17)
br label %Label1
Label1:
%19 = load i32, i32* %5, align 4
%20 = icmp slt i32 %19, %18
br i1 %20, label %Label6, label %Label2
Label6:
%21 = load i32*, i32** %4, align 8
%22 = load i32, i32* %5, align 4
%23 = getelementptr inbounds i32, i32* %21, i32 %22
%24 = load i32, i32* %23, align 4
store i32 %24, i32* %6, align 4
%25 = load i32, i32* %6, align 4
call void @printInt(i32 noundef %25)
%26 = load i32, i32* %5, align 4
%27 = add nsw i32 %26, 1
store i32 %27, i32* %5, align 4
br label %Label1
Label2:
%28 = load i32, i32* %3, align 4
%29 = add nsw i32 %28, 1
store i32 %29, i32* %3, align 4
br label %Label0
Label3:
ret void
}
define dso_local void @run() {
%1 = call noundef i32 @readInt()
%2 = call noundef i32 @readInt()
%3 = call noundef i32** @makeArray(i32 noundef %1,i32 noundef %2)
call void @printArray(i32** noundef %3)
ret void
}
define dso_local noundef i32 @main() {
call void @run()
ret i32 0
}
declare dso_local noundef i8* @addStrings(i8* noundef %0, i8* noundef %1)
declare dso_local void @printInt(i32 noundef %0)
declare dso_local void @printString(i8* noundef %0)
declare dso_local noundef i8* @readString()
declare dso_local noundef i32 @readInt()
declare dso_local void @increaseStringCounter(i8* noundef %0)
declare dso_local void @decreaseStringCounter(i8* noundef %0)
declare dso_local i32 @getArrayLength(i8* noundef %0)
declare dso_local i8* @allocArray(i32 noundef %0, i32 noundef %1)
declare dso_local zeroext i1 @isConstant(i8* noundef %0)
declare dso_local i32 @getReferenceCount(i8* noundef %0)
declare dso_local zeroext i1 @compareStrings(i8* noundef %0, i8* noundef %1)
declare dso_local i8* @allocateInstance(i8* noundef %0, i32 noundef %1)
declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)
