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
            Console.WriteLine("Введите ID процесса: ");
            int id = int.Parse(Console.ReadLine());
            Console.WriteLine("Введите имя модуля: ");
            string nameModule = Console.ReadLine();
            Init();
            Console.WriteLine($"Адрес: {GetBaseAddress(id, $"{nameModule}w")}");
            Console.Read();
        }
    }
}
