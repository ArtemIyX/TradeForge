using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Entities
{
    public class MinorStrategyStats
    {
        public double WinLosRatio { get; set; } = 0.0;
        public double AHPR { get; set; } = 0.0;
        public double Expectancy { get; set; } = 0.0;
        public double StagnationDays{ get; set; } = 0.0;
        public double StagnationPercentage { get; set; } = 0.0;

        /// <summary>
        /// Avg Win/Loss
        /// </summary>
        public double PayoutRatio { get; set; } = 0.0;
        public double ZScore { get; set; } = 0.0;
        public double ZProbability { get; set; } = 0.0;
        public double Deviation { get; set; } = 0.0;
        public double AverageNumberOfBarsInTrade { get; set; } = 0.0;
        public double Exposure { get; set; } = 0.0;
    }
}
