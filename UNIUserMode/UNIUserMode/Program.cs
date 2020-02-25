using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
namespace UNIUserMode
{
    class Program
    {
        [DllImport("UNIClient.dll")]
        private extern static bool Init();
        [DllImport("UNIClient.dll")]
        private extern static UInt64 GetBaseAddress(int ProcessID, string nameModule);

        static void Main(string[] args)
        {
            Console.WriteLine("Enter PID: ");
            int id = int.Parse(Console.ReadLine());
            Console.WriteLine("Enter name DLL (Must be placed with executables: ");
            string nameModule = Console.ReadLine();
            Init();
            Console.WriteLine($"Address at: {GetBaseAddress(id, $"{nameModule}w")}");
            Console.Read();
        }
    }
}
