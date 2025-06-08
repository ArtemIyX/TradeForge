using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Interfaces
{
    public interface IHistoryDataConverter<To, From> 
    {
        public To Convert(From origin);
        public ICollection<To> Convert(ICollection<From> origin);
    }
}
