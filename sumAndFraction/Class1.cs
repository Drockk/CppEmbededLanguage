using System.Runtime.InteropServices;

namespace sumAndFraction;

internal static class BinaryTrees
{
    [StructLayout(LayoutKind.Sequential)]
    public struct SumArgs
    {
        public readonly int a;
        public readonly int b;
    }

    public static int Sum(IntPtr arg, int argLength)
    {
        if (argLength < Marshal.SizeOf(typeof(SumArgs)))
        {
            return 0;
        }

        var libArgs = Marshal.PtrToStructure<SumArgs>(arg);

        return libArgs.a + libArgs.b;
    }

    private class TreeNode
    {
        private readonly TreeNode? _left;
        private readonly TreeNode? _right;

        private TreeNode(TreeNode? left = null, TreeNode? right = null)
        {
            this._left = left;
            this._right = right;
        }

        internal static TreeNode Create(int d)
        {
            return d == 0 ? new TreeNode()
                : new TreeNode(Create(d - 1), Create(d - 1));
        }

        internal int Check()
        {
            var c = 1;
            if (_right != null) c += _right.Check();
            if (_left != null) c += _left.Check();
            return c;
        }
    }

    private const int MinDepth = 4;
    
    [StructLayout(LayoutKind.Sequential)]
    public struct FactorialArgs
    {
        public readonly int n;
    }

    public static int BinaryTreePub(IntPtr arg, int argLength)
    {
        if (argLength < Marshal.SizeOf(typeof(FactorialArgs)))
        {
            return 0;
        }

        var libArgs = Marshal.PtrToStructure<FactorialArgs>(arg);

        BinaryTreePriv(libArgs.n);
        
        return 0;
    }

    private static void BinaryTreePriv(int nN)
    {
        var stretchDepth = nN + 1;
        Console.WriteLine($"stretch tree of depth {stretchDepth}\t check: {TreeNode.Create(stretchDepth).Check()}");

        var longLivedTree = TreeNode.Create(nN);
        var nResults = (nN - MinDepth) / 2 + 1;
        for (var i = 0; i < nResults; i++)
        {
            var depth = i * 2 + MinDepth;
            var n = (1 << nN - depth + MinDepth);

            var check = 0;
            for (var j = 0; j < n; j++)
            {
                check += TreeNode.Create(depth).Check();
            }

            Console.WriteLine($"{n}\t trees of depth {depth}\t check: {check}");
        }

        Console.WriteLine($"long lived tree of depth {nN}\t check: {longLivedTree.Check()}");
    }
}
