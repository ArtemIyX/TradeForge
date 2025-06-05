using RoboforexAPI.Models.Enums;

namespace RoboforexAPI.Extensions
{
    public static class RoboforexStringExtensions
    {
        public static TradeMode ToTradeMode(this string str)
            => str.ToLower() switch
            {
                "full" => TradeMode.Full,
                "buy_only" => TradeMode.BuyOnly,
                "sell_only" => TradeMode.SellOnly,
                "close_only" => TradeMode.CloseOnly,
                "disabled" => TradeMode.Disabled,
                _ => TradeMode.Disabled // Fallback for invalid values
            };

        public static AccountType ToAccountType(this string str)
            => str.ToLower() switch
            {
                "real" => AccountType.Real,
                "demo" => AccountType.Demo,
                _ => AccountType.Demo
            };

        public static AccountTradeSystem ToAccountTradeSystem(this string str)
           => str.ToLower() switch
           {
               "hedging" => AccountTradeSystem.Hedging,
               "netting" => AccountTradeSystem.Netting,
               _ => AccountTradeSystem.Hedging
           };

        public static AccountStatus ToAccountStatus(this string str)
          => str.ToLower() switch
          {
              "enabled" => AccountStatus.Enabled,
              "disabled" => AccountStatus.Disabled,
              "no_new_deals" => AccountStatus.NoNewDeals,
              _ => AccountStatus.Disabled
          };

        public static OrderType ToOrderType(this string str)
         => str.ToLower() switch
         {
             "market" => OrderType.Market,
             "stop" => OrderType.Stop,
             "limit" => OrderType.Limit,
             "stop_out" => OrderType.StopOut,
             "stop_loss" => OrderType.StopLoss,
             "take_profit " => OrderType.TakeProfit,
             _ => OrderType.Market
         };

        public static OrderSide ToOrderSide(this string str)
            => str.ToLower() switch
            {
                "buy" => OrderSide.Buy,
                "sell" => OrderSide.Sell,
                _ => OrderSide.Buy
            };

        public static OrderStatus ToOrderStatus(this string str)
            => str.ToLower() switch
            {
                "in_execution" => OrderStatus.InExecution,
                "active" => OrderStatus.Active,
                "rejected" => OrderStatus.Rejected,
                "filled" => OrderStatus.Filled,
                "canceled " => OrderStatus.Canceled,
                _ => OrderStatus.Rejected
            };

        public static DealStatus ToDealStatus(this string str)
           => str.ToLower() switch
           {
               "open" => DealStatus.Open,
               "closing" => DealStatus.Closing,
               "closed" => DealStatus.Closed,
               "trade" => DealStatus.Trade,
               _ => DealStatus.Trade
           };
    }
}
