using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Entities
{
    public class MonthlyStrategyPerformance
    {
        // Dictionary to store profits by year and month (Year -> Month -> Profit)
        private readonly Dictionary<int, Dictionary<int, decimal>> yearlyMonthlyProfits;

        public MonthlyStrategyPerformance()
        {
            yearlyMonthlyProfits = new Dictionary<int, Dictionary<int, decimal>>();
        }

        /// Set profit for a specific year and month
        public void SetMonthlyProfit(int year, int month, decimal profit)
        {
            if (month < 1 || month > 12)
                throw new ArgumentException("Month must be between 1 and 12", nameof(month));

            if (!yearlyMonthlyProfits.ContainsKey(year))
            {
                yearlyMonthlyProfits[year] = new Dictionary<int, decimal>();
            }

            yearlyMonthlyProfits[year][month] = profit;
        }

        // Get profit for a specific year and month
        public decimal GetMonthlyProfit(int year, int month)
        {
            if (month < 1 || month > 12)
                throw new ArgumentException("Month must be between 1 and 12", nameof(month));

            if (yearlyMonthlyProfits.ContainsKey(year) && yearlyMonthlyProfits[year].ContainsKey(month))
            {
                return yearlyMonthlyProfits[year][month];
            }

            return 0m; // Return 0 if no data exists for the specified year/month
        }

        // Calculate total profit for a specific year
        public decimal GetYearlyTotal(int year)
        {
            if (!yearlyMonthlyProfits.ContainsKey(year))
                return 0m;

            return yearlyMonthlyProfits[year].Values.Sum();
        }

        // Get all years with data
        public IEnumerable<int> GetYears()
        {
            return yearlyMonthlyProfits.Keys.OrderBy(y => y);
        }

        // Get all months with data for a specific year
        public IEnumerable<int> GetMonthsForYear(int year)
        {
            if (!yearlyMonthlyProfits.ContainsKey(year))
                return Enumerable.Empty<int>();

            return yearlyMonthlyProfits[year].Keys.OrderBy(m => m);
        }
    }
}
