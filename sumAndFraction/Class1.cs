using System;
using System.Runtime.InteropServices;

namespace SumAndFraction
{
    public static class Lib
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct SumArgs
        {
            public int a;
            public int b;
        }

        public static int Sum(IntPtr arg, int argLength)
        {
            if (argLength < System.Runtime.InteropServices.Marshal.SizeOf(typeof(SumArgs)))
            {
                return 0;
            }

            SumArgs libArgs = Marshal.PtrToStructure<SumArgs>(arg);

            return libArgs.a + libArgs.b;
        }

        public struct FactorialArgs
        {
            public int n;
        }

        public static int FactorialPub(IntPtr arg, int argLength)
        {
            if (argLength < System.Runtime.InteropServices.Marshal.SizeOf(typeof(FactorialArgs)))
            {
                return 0;
            }

            FactorialArgs libArgs = Marshal.PtrToStructure<FactorialArgs>(arg);

            return Factorial(libArgs.n);
        }

        private static int Factorial(int n)
        {
            if (n < 0)
                Console.Write("Wrong argument");
            if (n > 1)
                return Factorial(n - 1) * n;
            else
                return 1;
        }
    }
}
