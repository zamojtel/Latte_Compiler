%class.A = type {i8*}
%class.B = type {%class.A,i32}
@.vtable.A = dso_local global [0 x i8*] [], align 16
@.vtable.B = dso_local global [0 x i8*] [], align 16
define dso_local noundef i32 @main() {
%1 = alloca %class.B*, align 8
%2 = alloca %class.A*, align 8
%3 = call i8* @allocateInstance(i8* noundef bitcast ([0 x i8*]* @.vtable.B to i8*),i32 noundef 4)
%4 = bitcast i8* %3 to %class.B*
store %class.B* %4, %class.B** %1, align 8
%5 = load %class.B*, %class.B** %1, align 8
%6 = getelementptr inbounds %class.B, %class.B* %5, i32 0, i32 1
store i32 42, i32* %6, align 4
%7 = load %class.B*, %class.B** %1, align 8
%8 = bitcast %class.B* %7 to %class.A*
store %class.A* %8, %class.A** %2, align 8
%9 = load %class.A*, %class.A** %2, align 8
%10 = getelementptr inbounds %class.A, %class.A* %9, i32 0, i32 0
%11 = load i8*, i8** %10, align 8
call void @printString(i8* noundef %11)
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
