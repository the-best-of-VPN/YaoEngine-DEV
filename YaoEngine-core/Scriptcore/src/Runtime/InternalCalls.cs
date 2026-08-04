using System.Runtime.CompilerServices;

namespace YaoEngine
{
    internal static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void NativeLog(string message);
    }
}
