using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzerConsoleExample
{
    public class QuantXDeal
    {
        [Name("Ticket")]
        public long Ticket { get; set; } = 0;

        [Name("Symbol")]
        public string Symbol { get; set; } = string.Empty;

        [Name("Type")]
        public string Type { get; set; } = string.Empty;

        [Name("Open time")]
        [Format("dd.MM.yyyy H:mm")]
        public DateTime OpenTime { get; set; } = DateTime.MinValue;

        [Name("Open price")]
        public double OpenPrice { get; set; } = 0.0;

        [Name("Size")]
        public double Size { get; set; } = 0.0;

        [Name("Close time")]
        [Format("dd.MM.yyyy H:mm")] // Specify date format
        public DateTime CloseTime { get; set; } = DateTime.MinValue;

        [Name("Close price")]
        public double ClosePrice { get; set; } = 0.0;

        [Name("Profit/Loss")]
        public double ProfitLoss { get; set; } = 0.0;

        [Name("Balance")]
        public double Balance { get; set; } = 0.0;

        [Name("Sample type")]
        public string SampleType { get; set; } = string.Empty;

        [Name("Close type")]
        public string CloseType { get; set; } = string.Empty;

        [Name("MAE ($)")]
        public double MAE { get; set; } = 0.0;

        [Name("MFE ($)")]
        public double MFE { get; set; } = 0.0;

        [Name("Time in trade")]
        public string TimeInTrade { get; set; } = string.Empty;

        [Name("Comment")]
        public string Comment { get; set; } = string.Empty;
    }
}
