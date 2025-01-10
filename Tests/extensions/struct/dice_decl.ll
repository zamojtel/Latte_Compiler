%class.Dice = type {i32,i8*}
define dso_local noundef i32 @main() {
%1 = alloca %class.Dice*, align 8
%2 = call i8* @allocateInstance(i32 noundef 12)
%3 = bitcast i8* %2 to %class.Dice*
store %class.Dice* %3, %class.Dice** %1, align 8
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
