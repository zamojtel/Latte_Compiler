@.str.6 = private unnamed_addr constant [11 x i8] c"\00\00\00\00\00ERROR\00", align 1
@.str.5 = private unnamed_addr constant [8 x i8] c"\00\00\00\00\00OK\00", align 1
@.str.2 = private unnamed_addr constant [9 x i8] c"\00\00\00\00\00ala\00", align 1
@.str.1 = private unnamed_addr constant [15 x i8] c"\00\00\00\00\00alamakota\00", align 1
@.str.4 = private unnamed_addr constant [10 x i8] c"\00\00\00\00\00kota\00", align 1
@.str.3 = private unnamed_addr constant [8 x i8] c"\00\00\00\00\00ma\00", align 1
define dso_local noundef i32 @main() {
%1 = alloca i8*, align 8
%2 = getelementptr inbounds [15 x i8], [15 x i8]* @.str.1, i32 0, i32 0
store i8* %2, i8** %1, align 8
%3 = getelementptr inbounds [9 x i8], [9 x i8]* @.str.2, i32 0, i32 0
%4 = getelementptr inbounds [8 x i8], [8 x i8]* @.str.3, i32 0, i32 0
%5 = call noundef i8* @addStrings(i8* noundef getelementptr ([9 x i8], [9 x i8]* @.str.2, i32 0, i32 0),i8* noundef getelementptr ([8 x i8], [8 x i8]* @.str.3, i32 0, i32 0))
%6 = getelementptr inbounds [10 x i8], [10 x i8]* @.str.4, i32 0, i32 0
%7 = call noundef i8* @addStrings(i8* noundef %5,i8* noundef getelementptr ([10 x i8], [10 x i8]* @.str.4, i32 0, i32 0))
%8 = load i8*, i8** %1, align 8
%9 = call zeroext i1 @compareStrings(i8* noundef %7, i8* noundef %8)
br i1 %9, label %Label3, label %Label0
Label3:
call void @printString(i8* noundef getelementptr ([8 x i8], [8 x i8]* @.str.5, i32 0, i32 0))
br label %Label1
Label0:
call void @printString(i8* noundef getelementptr ([11 x i8], [11 x i8]* @.str.6, i32 0, i32 0))
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
declare dso_local i8* @allocateInstance(i8* noundef %0, i32 noundef %1)
declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)
