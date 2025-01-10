%class.Dice = type {i32,i8*,i8}
@.str.1 = private unnamed_addr constant [16 x i8] c"\00\00\00\00\00Octahedron\00", align 1
@.str.2 = private unnamed_addr constant [11 x i8] c"\00\00\00\00\00false\00", align 1
@.str.3 = private unnamed_addr constant [10 x i8] c"\00\00\00\00\00true\00", align 1
define dso_local noundef i32 @main() {
%1 = alloca %class.Dice*, align 8
store %class.Dice* null, %class.Dice** %1, align 8
%2 = call i8* @allocateInstance(i32 noundef 13)
%3 = bitcast i8* %2 to %class.Dice*
store %class.Dice* %3, %class.Dice** %1, align 8
%4 = load %class.Dice*, %class.Dice** %1, align 8
%5 = getelementptr inbounds %class.Dice, %class.Dice* %4, i32 0, i32 0
store i32 6, i32* %5, align 4
%6 = load %class.Dice*, %class.Dice** %1, align 8
%7 = getelementptr inbounds %class.Dice, %class.Dice* %6, i32 0, i32 1
%8 = getelementptr inbounds [16 x i8], [16 x i8]* @.str.1, i32 0, i32 0
store i8* %8, i8** %7, align 8
%9 = load %class.Dice*, %class.Dice** %1, align 8
%10 = getelementptr inbounds %class.Dice, %class.Dice* %9, i32 0, i32 2
%11 = zext i1 1 to i8
store i8 %11, i8* %10, align 1
%12 = load %class.Dice*, %class.Dice** %1, align 8
%13 = getelementptr inbounds %class.Dice, %class.Dice* %12, i32 0, i32 0
%14 = load i32, i32* %13, align 4
call void @printInt(i32 noundef %14)
%15 = load %class.Dice*, %class.Dice** %1, align 8
%16 = getelementptr inbounds %class.Dice, %class.Dice* %15, i32 0, i32 1
%17 = load i8*, i8** %16, align 8
call void @printString(i8* noundef %17)
%18 = load %class.Dice*, %class.Dice** %1, align 8
%19 = getelementptr inbounds %class.Dice, %class.Dice* %18, i32 0, i32 2
%20 = load i8, i8* %19, align 1
%21 = trunc i8 %20 to i1
call void @printBool(i1 noundef %21)
ret i32 0
}
define dso_local void @printBool(i1 noundef zeroext %0) {
%2 = alloca i8, align 1
%3 = alloca i8, align 1
%4 = zext i1 %0 to i8
store i8 %4, i8* %2, align 1
%5 = load i8, i8* %2, align 1
%6 = trunc i8 %5 to i1
br i1 %6, label %Label5, label %Label0
Label5:
%7 = zext i1 0 to i8
store i8 %7, i8* %3, align 1
br label %Label1
Label0:
%8 = zext i1 1 to i8
store i8 %8, i8* %3, align 1
br label %Label1
Label1:
%9 = load i8, i8* %3, align 1
%10 = trunc i8 %9 to i1
br i1 %10, label %Label6, label %Label2
Label6:
call void @printString(i8* noundef getelementptr ([11 x i8], [11 x i8]* @.str.2, i32 0, i32 0))
br label %Label3
Label2:
call void @printString(i8* noundef getelementptr ([10 x i8], [10 x i8]* @.str.3, i32 0, i32 0))
br label %Label3
Label3:
ret void
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
declare dso_local i8* @allocateInstance(i32 noundef %0)
declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)
