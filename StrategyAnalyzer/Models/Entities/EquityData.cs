using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Entities
{
    public class EquityDataItem
    {
        public long CloseTime { get; set; } = 0;
        public double Profit { get; set; } = 0.0;
    }

    public class EquityData
    {
        public List<EquityDataItem> Items { get; set; } = [];

        public List<long> Indecies => [.. Items.Select(x => x.CloseTime)];
        public List<double> Profits => [.. Items.Select(x => x.Profit)];

        public List<double> Balance(double initialBalance)
        {
            List<double> balances = [];
            double currentBalance = initialBalance;
            foreach (var item in Items)
            {
                currentBalance += item.Profit;
                balances.Add(currentBalance);
            }
            return balances;
        }
    }
}
