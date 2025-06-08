using Microsoft.Extensions.Logging;
using RoboforexAPI.Models.Entities;
using RoboforexAPI.Models.Enums;
using StrategyAnalyzer.Models.Entities;
using StrategyAnalyzer.Models.Interfaces;

namespace StrategyAnalyzer.Services
{
    public class AnalyzerService(ILogger<AnalyzerService> logger) : IStrategyAnalyzer<Deal, AnalyzerConfig>
    {
        protected readonly ILogger<AnalyzerService> _logger = logger;

        protected ICollection<Deal>? _deals;

        private double _initialBalance = 10000;
        public double InitialBalance { get => _initialBalance; set => _initialBalance = value; }

        private AnalyzerConfig _config = new AnalyzerConfig();
        public AnalyzerConfig StrategyConfig { get => _config; set => _config = value; }

        public ICollection<Deal> GetListOfTrades()
        {
            if (_deals == null)
            {
                return new List<Deal>();
            }
            return _deals;
        }

        public void ImportListOfTrades(ICollection<Deal> history)
        {
            _deals = history;
        }

        public Task<EquityData> Equity(CancellationToken cancellationToken = default)
        {
            return Task.Run(() =>
            {
                var equityData = new EquityData();

                if (_deals == null || !_deals.Any())
                {
                    return Task.FromResult(equityData);
                }

                // Filter closed deals and sort by close time
                var closedDeals = _deals
                    .Where(d => d.StatusEnum == DealStatus.Closed && d.ClosePrice.HasValue && d.CloseTime.HasValue)
                    .OrderBy(d => d.CloseTime)
                    .ToList();

                var items = new List<EquityDataItem>();

                foreach (var deal in closedDeals)
                {
                    if (deal.ClosePrice.HasValue)
                    {
                        // Calculate profit for the deal
                        double profit = CalculateDealProfit(deal, _config.InstrumentFullDescription, _config.InstrumentShortDescription);

                        items.Add(new EquityDataItem
                        {
                            CloseTime = deal.CloseTime!.Value,
                            Profit = profit
                        });
                    }
                }

                equityData.Items = items;
                return Task.FromResult(equityData);
            });
        }

        private static double CalculateDealProfit(Deal deal,
            RoboforexAPI.Models.Entities.Instrument instrument,
            InstrumentShort instrumentShort)
        {
            if (instrument == null)
                throw new ArgumentNullException(nameof(instrument), "Instrument cannot be null.");
            if (instrumentShort == null)
                throw new ArgumentNullException(nameof(instrumentShort), "InstrumentShort cannot be null.");
            return CalculateDealProfit(deal, instrumentShort.ContractSize, instrument.MinTick);
        }

        private static double CalculateDealProfit(Deal deal, double contractSize, double minTick)
        {
            // Validate input parameters
            if (deal == null)
                throw new ArgumentNullException(nameof(deal), "Deal cannot be null.");

            if (deal.ClosePrice == null)
                throw new InvalidOperationException("Cannot calculate profit: ClosePrice is null.");

            if (deal.Volume <= 0)
                throw new ArgumentException("Volume must be greater than zero.", nameof(deal.Volume));

            if (!deal.ClosePrice.HasValue)
                throw new ArgumentException("Close price should be not null", nameof(deal.Volume));

            if (deal.OpenPrice < 0 || deal.ClosePrice < 0)
                throw new ArgumentException("Prices cannot be negative.", nameof(deal.OpenPrice));

            if (minTick <= 0 || contractSize <= 0)
                throw new ArgumentException("MinTick and ContractSize must be greater than zero.");

            // Calculate point cost: ContractSize * MinTick
            double pointCost = contractSize * minTick;

            // Calculate price difference based on deal side
            double priceDifference;
            if (deal.SideEnum == OrderSide.Buy)
            {
                priceDifference = deal.ClosePrice.Value - deal.OpenPrice;
            }
            else if (deal.SideEnum == OrderSide.Sell)
            {
                priceDifference = deal.OpenPrice - deal.ClosePrice.Value;
            }
            else
            {
                throw new ArgumentException("Invalid SideEnum value.", nameof(deal.SideEnum));
            }

            // Calculate number of points
            double points = priceDifference / minTick;

            // Calculate profit: points * point cost * volume
            return points * pointCost * deal.Volume;
        }

        public Task<MainStrategyStats> Main(CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<MinorStrategyStats> Minor(CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<MonthlyStrategyPerformance> MonthlyPerformance(CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<TradeStats> Trade(CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }
    }
}
