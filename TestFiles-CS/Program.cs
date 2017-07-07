using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestFiles_CS
{
  class Program
  {
    public int aPublicDataMember = 3;
    static void Main(string[] args)
    {
      Program p = new Program();
      Console.Write("\n  aPublicDataMember = {0}\n", p.aPublicDataMember);
    }
  }
}
