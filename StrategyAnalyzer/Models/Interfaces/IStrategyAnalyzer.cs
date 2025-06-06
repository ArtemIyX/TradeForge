using StrategyAnalyzer.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Interfaces
{
    public interface IStrategyAnalyzer<T>
    {
        public Task ImportListOfTrades(ICollection<T> history);
        public ICollection<T> GetListOfTrades();
        public Task<MainStrategyStats> Main();
        public Task<MinorStrategyStats> Minor();
        public Task<TradeStats> Trade();
        public Task<MonthlyStrategyPerformance> MonthlyPerformance();
    }
}
