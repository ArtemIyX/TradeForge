using Microsoft.Extensions.Logging;
using RoboforexAPI.Models.Entities;
using StrategyAnalyzer.Models.Entities;
using StrategyAnalyzer.Models.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Services
{
    public class AnalyzerService(ILogger<AnalyzerService> logger) : IStrategyAnalyzer<Deal>
    {
        protected readonly ILogger<AnalyzerService> _logger = logger;

        public ICollection<Deal> GetListOfTrades()
        {
            throw new NotImplementedException();
        }

        public Task ImportListOfTrades(ICollection<Deal> history)
        {
            throw new NotImplementedException();
        }

        public Task<MainStrategyStats> Main()
        {
            throw new NotImplementedException();
        }

        public Task<MinorStrategyStats> Minor()
        {
            throw new NotImplementedException();
        }

        public Task<MonthlyStrategyPerformance> MonthlyPerformance()
        {
            throw new NotImplementedException();
        }

        public Task<TradeStats> Trade()
        {
            throw new NotImplementedException();
        }
    }
}
