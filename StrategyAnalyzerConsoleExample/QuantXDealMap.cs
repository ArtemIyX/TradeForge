using CsvHelper.Configuration;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzerConsoleExample
{
    public class QuantXDealMap : ClassMap<QuantXDeal>
    {
        public QuantXDealMap()
        {
            Map(m => m.Ticket).Name("Ticket");
            Map(m => m.Symbol).Name("Symbol");
            Map(m => m.Type).Name("Type");
            Map(m => m.OpenTime).Name("Open time").TypeConverterOption.Format("dd.MM.yyyy H:mm");
            Map(m => m.OpenPrice).Name("Open price");
            Map(m => m.Size).Name("Size");
            Map(m => m.CloseTime).Name("Close time").TypeConverterOption.Format("dd.MM.yyyy H:mm");
            Map(m => m.ClosePrice).Name("Close price");
            Map(m => m.ProfitLoss).Name("Profit/Loss");
            Map(m => m.Balance).Name("Balance");
            Map(m => m.SampleType).Name("Sample type");
            Map(m => m.CloseType).Name("Close type");
            Map(m => m.MAE).Name("MAE ($)");
            Map(m => m.MFE).Name("MFE ($)");
            Map(m => m.TimeInTrade).Name("Time in trade");
            Map(m => m.Comment).Name("Comment");
        }
    }
}
