using RoboforexAPI.Models.Entities;
using RoboforexAPI.Models.Requests;
using RoboforexAPI.Models.Requests.Trading;
using RoboforexAPI.Models.Responses.Instruments;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Interfaces
{
    public interface IBrokerAPI
    {
        public string ApiUrl { get; set; }
        public string ApiToken { get; set; }

        public Task Logout(CancellationToken cancellationToken = default);

        public Task<InstrumentShort[]> GetInstruments(CancellationToken cancellationToken = default);

        public Task<Instrument> GetInstrumentDescription(
             AccountTickerRequest request, CancellationToken cancellationToken = default);

        public Task<QuoteData> GetQuoteData(
             AccountTickerRequest request, CancellationToken cancellationToken = default);

        public Task<Account[]> GetAccounts(CancellationToken cancellationToken = default);

        public Task<AccountState> GetAccountState(BaseAccountRequest request,
            CancellationToken cancellationToken = default);

        public Task<Order[]> GetOrders(BaseAccountRequest request,
           CancellationToken cancellationToken = default);

        public Task<Deal[]> GetDeals(BaseAccountRequest request,
          CancellationToken cancellationToken = default);

        public Task PlaceOrder(PlaceOrderRequest request,
            CancellationToken cancellationToken = default);

        public Task ModifyOrder(ModifyOrderRequest request,
            CancellationToken cancellationToken = default);

        public Task CancelOrder(CancelOrderRequest request,
           CancellationToken cancellationToken = default);

        public Task ModifyDeal(ModifyDealRequest request,
           CancellationToken cancellationToken = default);

        public Task CloseDeal(CloseDealRequest request,
         CancellationToken cancellationToken = default);
    }
}
