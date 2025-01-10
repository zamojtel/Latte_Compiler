@.str.3 = private unnamed_addr constant [11 x i8] c"\00\00\00\00\00Equal\00", align 1
@.str.4 = private unnamed_addr constant [15 x i8] c"\00\00\00\00\00Not Equal\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"\00\00\00\00\00abc\00", align 1
define dso_local noundef i32 @main() {
%1 = alloca i8*, align 8
%2 = alloca i8*, align 8
%3 = getelementptr inbounds [9 x i8], [9 x i8]* @.str.1, i32 0, i32 0
store i8* %3, i8** %1, align 8
%4 = getelementptr inbounds [9 x i8], [9 x i8]* @.str.1, i32 0, i32 0
store i8* %4, i8** %2, align 8
%5 = load i8*, i8** %1, align 8
%6 = load i8*, i8** %2, align 8
%7 = call zeroext i1 @compareStrings(i8* noundef %5, i8* noundef %6)
br i1 %7, label %Label3, label %Label0
Label3:
call void @printString(i8* noundef getelementptr ([11 x i8], [11 x i8]* @.str.3, i32 0, i32 0))
br label %Label1
Label0:
call void @printString(i8* noundef getelementptr ([15 x i8], [15 x i8]* @.str.4, i32 0, i32 0))
br label %Label1
Label1:
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
declare dso_local i8* @allocateInstance(i32 noundef %0)
declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)
