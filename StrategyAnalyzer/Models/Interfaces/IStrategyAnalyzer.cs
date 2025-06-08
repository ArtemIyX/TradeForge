using StrategyAnalyzer.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Interfaces
{
    public interface IStrategyAnalyzer<Deal, Config>
    {
        public Config StrategyConfig { get; set; }
        public void ImportListOfTrades(ICollection<Deal> history);
        public ICollection<Deal> GetListOfTrades();

        public Task<EquityData> Equity(CancellationToken cancellationToken = default);
        public Task<MainStrategyStats> Main(CancellationToken cancellationToken = default);
        public Task<MinorStrategyStats> Minor(CancellationToken cancellationToken = default);
        public Task<TradeStats> Trade(CancellationToken cancellationToken = default);
        public Task<MonthlyStrategyPerformance> MonthlyPerformance(CancellationToken cancellationToken = default);
    }
}
