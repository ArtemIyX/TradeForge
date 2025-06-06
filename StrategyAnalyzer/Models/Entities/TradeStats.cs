using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Entities
{
    public class TradeStats
    {
        public double GrossProfit { get; set; } = 0.0;
        public double GrossLoss { get; set; } = 0.0;

        public double LargestWin { get; set; } = 0.0;
        public double LargestLoss { get; set; } = 0.0;

        public int NumberOfWins { get; set; } = 0;
        public int NumberOfLosses { get; set; } = 0;

        public double AverageWin { get; set; } = 0.0;
        public double AverageLoss { get; set; } = 0.0;

        public int MaxConsecWins { get; set; } = 0;
        public int MaxConsecLosses { get; set; } = 0;

        public double AvgBarInWins { get; set; } = 0.0;
        public double AvgBarInLosses { get; set; } = 0.0;

    }
}
