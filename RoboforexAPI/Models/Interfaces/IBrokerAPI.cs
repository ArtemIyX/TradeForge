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
    /// <summary>
    /// Provides an abstraction for communication with a trading API.
    /// </summary>
    public interface IBrokerAPI
    {
        /// <summary>
        /// Gets or sets the base URL of the API.
        /// </summary>
        public string ApiUrl { get; set; }

        /// <summary>
        /// Gets or sets the authentication token used for API access.
        /// </summary>
        public string ApiToken { get; set; }

        /// <summary>
        /// Logs out the current session.
        /// </summary>
        public Task Logout(CancellationToken cancellationToken = default);

        /// <summary>
        /// Retrieves a list of available instruments.
        /// </summary>
        public Task<InstrumentShort[]> GetInstruments(CancellationToken cancellationToken = default);

        /// <summary>
        /// Gets detailed information about a specific instrument.
        /// </summary>
        /// <param name="request">The request containing the account and ticker.</param>
        public Task<Instrument> GetInstrumentDescription(
             AccountTickerRequest request, CancellationToken cancellationToken = default);

        /// <summary>
        /// Retrieves quote data for a given instrument.
        /// </summary>
        /// <param name="request">The request containing the account and ticker.</param>
        public Task<QuoteData> GetQuoteData(
             AccountTickerRequest request, CancellationToken cancellationToken = default);

        /// <summary>
        /// Retrieves all accounts associated with the current session.
        /// </summary>
        public Task<Account[]> GetAccounts(CancellationToken cancellationToken = default);

        /// <summary>
        /// Gets the state of a specified account.
        /// </summary>
        /// <param name="request">The account request.</param>
        public Task<AccountState> GetAccountState(BaseAccountRequest request,
            CancellationToken cancellationToken = default);

        /// <summary>
        /// Retrieves all active and historical orders for the specified account.
        /// </summary>
        /// <param name="request">The account request.</param>
        public Task<Order[]> GetOrders(BaseAccountRequest request,
           CancellationToken cancellationToken = default);

        /// <summary>
        /// Retrieves all deals executed in the specified account.
        /// </summary>
        /// <param name="request">The account request.</param>
        public Task<Deal[]> GetDeals(BaseAccountRequest request,
          CancellationToken cancellationToken = default);

        /// <summary>
        /// Places a new order.
        /// </summary>
        /// <param name="request">The request containing order parameters.</param>
        public Task<OrderId> PlaceOrder(PlaceOrderRequest request,
            CancellationToken cancellationToken = default);

        /// <summary>
        /// Modifies an existing order.
        /// </summary>
        /// <param name="request">The modification request.</param>
        public Task ModifyOrder(ModifyOrderRequest request,
            CancellationToken cancellationToken = default);

        /// <summary>
        /// Cancels an existing order.
        /// </summary>
        /// <param name="request">The cancellation request.</param>
        public Task CancelOrder(CancelOrderRequest request,
           CancellationToken cancellationToken = default);

        /// <summary>
        /// Modifies an existing deal.
        /// </summary>
        /// <param name="request">The modification request.</param>
        public Task ModifyDeal(ModifyDealRequest request,
           CancellationToken cancellationToken = default);

        /// <summary>
        /// Closes an open deal.
        /// </summary>
        /// <param name="request">The close deal request.</param>
        public Task CloseDeal(CloseDealRequest request,
         CancellationToken cancellationToken = default);
    }
}
