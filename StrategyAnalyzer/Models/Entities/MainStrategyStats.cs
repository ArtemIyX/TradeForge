using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Entities
{
    public class MainStrategyStats
    {
        public int NumberOfTrades { get; set; } = 0;
        public double SharpRatio { get; set; } = 0.0;
        public double SortinoRatio { get; set; } = 0.0;
        public double ProfitFactor { get; set; } = 0.0;
        public double ReturnDrawDawnRatio { get; set; } = 0.0;
        public double WinningPercentage { get; set; } = 0.0;
        public double DrawDown { get; set; } = 0.0;
        public double DrawDownPercentage { get; set; } = 0.0;
        public double DailyAvgProfit { get; set; } = 0.0;
        public double MonthlyAvgProfit { get; set; } = 0.0;
        public double AverageTrade { get; set; } = 0.0;

        /// <summary>
        /// Annual % / Max DD%
        /// </summary>
        public double AnnualMaxDddRatio { get; set; } = 0.0;

        public double RExpectancy { get; set; } = 0.0;
        public double RExpectancyScore { get; set; } = 0.0;
        public double StrQualityNumber { get;set; } = 0.0;
        public double SqnScore { get; set;} = 0.0;
    }
}
