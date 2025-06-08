using RoboforexAPI.Models.Entities;
using RoboforexAPI.Models.Enums;
using StrategyAnalyzer.Models.Entities;
using StrategyAnalyzer.Models.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzerConsoleExample
{
    public class QuantXConverter : IHistoryDataConverter<Deal, QuantXDeal>
    {
        public ICollection<Deal> Convert(ICollection<QuantXDeal> origin)
        {
            return [.. origin.Select(Convert)];
        }

        public Deal Convert(QuantXDeal origin)
        {
            return new Deal()
            {
                Id = origin.Ticket.ToString(),
                Ticker = origin.Symbol,
                Volume = origin.Size,
                SideStr = origin.CloseType == "Buy" ? "buy" : "sell",
                OpenPrice = origin.OpenPrice,
                OpenTime = origin.OpenTime.ToUniversalTime().Ticks,
                ClosePrice = origin.ClosePrice,
                CloseTime = origin.CloseTime.ToUniversalTime().Ticks,
                StatusStr = "closed"
            };
        }
    }
}
