using Microsoft.Extensions.Logging;
using RoboforexAPI.Extensions;
using RoboforexAPI.Models.Entities;
using RoboforexAPI.Models.Interfaces;
using RoboforexAPI.Models.Requests;
using RoboforexAPI.Models.Requests.Accounts;
using RoboforexAPI.Models.Requests.Trading;
using RoboforexAPI.Models.Responses;
using RoboforexAPI.Models.Responses.Accounts;
using RoboforexAPI.Models.Responses.Instruments;
using RoboforexAPI.Models.Responses.Trading;
using System.Net.Http.Json;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace RoboforexAPI.Services
{
    /// <summary>
    /// RoboforexService provides an implementation of the <see cref="IBrokerAPI"/> interface 
    /// to interact with the StocksTrader broker API.
    /// 
    /// This service uses `HttpClient` to send authenticated requests and handle responses 
    /// from the API endpoints listed at:
    /// https://api-doc.stockstrader.com/
    /// 
    /// </summary>
    public class RoboforexService(
        HttpClient httpClient,
        ILogger<RoboforexService> logger) : IBrokerAPI
    {
        protected readonly HttpClient _httpClient = httpClient;
        protected readonly ILogger<RoboforexService> _logger = logger;

        private string _api = "https://api.stockstrader.com/api/v1/";
        public string ApiUrl { get => _api; set => _api = value; }
        public string ApiToken { get; set; } = string.Empty;

        #region Http

        /// <summary>
        /// Adds the Authorization header to the request using the bearer token.
        /// </summary>
        /// <param name="request">The HTTP request message.</param>
        protected void AddAuth(HttpRequestMessage request)
        {
            if (!string.IsNullOrEmpty(ApiToken))
            {
                request.Headers.Add("Authorization", "Bearer " + ApiToken);
            }
        }

        /// <summary>
        /// Parses and validates the HTTP response from the API.
        /// Throws exceptions for HTTP and application-level errors.
        /// </summary>
        /// <param name="response">The HTTP response message.</param>
        /// <param name="cancellationToken">Cancellation token.</param>
        /// <returns>A deserialized <see cref="BaseResponse"/> object.</returns>
        protected async Task<BaseResponse> FetchResponse(HttpResponseMessage response,
            CancellationToken cancellationToken = default)
        {
            if (response.StatusCode == System.Net.HttpStatusCode.Unauthorized)
            {
                throw new HttpRequestException("Unauthorized: Invalid or missing authentication credentials.",
                    null, System.Net.HttpStatusCode.Unauthorized);
            }

            if(response.StatusCode == System.Net.HttpStatusCode.BadRequest)
            {
                throw new HttpRequestException($"Bad request: {await response.Content.ReadAsStringAsync()}",
                   null, System.Net.HttpStatusCode.BadRequest);
            }

            // Read and deserialize the response content
            string jsonResponse = await response.Content.ReadAsStringAsync(cancellationToken);
            BaseResponse? parsedResponse = JsonSerializer.Deserialize<BaseResponse>(jsonResponse);

            // Verify deserialization result
            if (parsedResponse == null)
            {
                throw new InvalidOperationException($"Failed to deserialize the response '{jsonResponse}'");
            }

            if (parsedResponse.Error)
            {
                throw new InvalidOperationException($"Request error: '{parsedResponse.ErrorMsg}'");
            }

            return parsedResponse;
        }

        /// <summary>
        /// Executes an HTTP GET request to the API.
        /// </summary>
        /// <param name="query">The endpoint path (relative to the base URL).</param>
        /// <param name="cancellationToken">Cancellation token.</param>
        /// <returns>Parsed <see cref="BaseResponse"/> object.</returns>
        protected async Task<BaseResponse> GetRequest(string query,
             CancellationToken cancellationToken = default)
        {
            try
            {
                if (string.IsNullOrEmpty(query))
                {
                    throw new ArgumentException("Query cannot be null or empty", nameof(query));
                }

                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, _api + query);
                AddAuth(request);
                HttpResponseMessage response = await _httpClient.SendAsync(request, cancellationToken);

                return await FetchResponse(response, cancellationToken);
            }
            catch (HttpRequestException ex)
            {
                throw new HttpRequestException($"HTTP request failed: {ex.Message}", ex);
            }
            catch (JsonException ex)
            {
                throw new InvalidOperationException($"Failed to parse response: {ex.Message}", ex);
            }
            catch (Exception ex)
            {
                throw new Exception($"An unexpected error occurred: {ex.Message}", ex);
            }
        }

        /// <summary>
        /// Type of HTTP request supported in <see cref="SendRequest"/>.
        /// </summary>
        public enum HttpRequestType
        {
            Post,
            Put,
            Delete
        }

        /// <summary>
        /// Executes an HTTP POST, PUT, or DELETE request with optional body.
        /// </summary>
        /// <param name="query">The endpoint path.</param>
        /// <param name="body">Form data to include in request.</param>
        /// <param name="requestType">HTTP method.</param>
        /// <param name="cancellationToken">Cancellation token.</param>
        /// <returns>Parsed <see cref="BaseResponse"/> object.</returns>
        protected async Task<BaseResponse> SendRequest(string query, 
            Dictionary<string, string>? body = null, 
            HttpRequestType requestType = HttpRequestType.Post,
            CancellationToken cancellationToken = default)
        {
            try
            {
                // Validate inputs
                if (string.IsNullOrEmpty(query))
                {
                    throw new ArgumentException("Query cannot be null or empty", nameof(query));
                }

                HttpMethod? method = null;
                // Handle different request types
                switch (requestType)
                {
                    case HttpRequestType.Post:
                        method = HttpMethod.Post;
                        break;
                    case HttpRequestType.Put:
                        method = HttpMethod.Put;
                        break;
                    case HttpRequestType.Delete:
                        method = HttpMethod.Delete;
                        break;
                    default:
                        throw new ArgumentException("Unsupported request type.", nameof(requestType));
                }

                string url = _api + query;
                HttpRequestMessage requestMessage = new HttpRequestMessage(method, url);
                AddAuth(requestMessage);
                if (body != null)
                {
                    requestMessage.Content = new FormUrlEncodedContent(body);
                }

                // Initialize response
                HttpResponseMessage? response = await _httpClient.SendAsync(requestMessage, cancellationToken);

                if (response == null)
                {
                    throw new InvalidOperationException("Response is invalid (null)");
                }

                return await FetchResponse(response, cancellationToken);
            }
            catch (HttpRequestException ex)
            {
                throw new HttpRequestException($"HTTP request failed: {ex.Message}", ex);
            }
            catch (JsonException ex)
            {
                throw new InvalidOperationException($"Failed to parse response: {ex.Message}", ex);
            }
            catch (Exception ex)
            {
                throw new Exception($"An unexpected error occurred: {ex.Message}", ex);
            }
        }

        #endregion

        #region IBrokerAPI

        /// <inheritdoc/>
        public async Task<Account[]> GetAccounts(CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest("accounts", cancellationToken);
            AccountsResponse accountsResponse = new AccountsResponse(response);
            return accountsResponse.Accounts;
        }

        /// <inheritdoc/>
        public async Task<AccountState> GetAccountState(BaseAccountRequest request, CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest($"accounts/{request.AccountId.Trim()}", cancellationToken);
            AccountStateResponse stateResponse = new AccountStateResponse(response);
            return stateResponse.AccountStateData;
        }

        /// <inheritdoc/>
        public async Task<Deal[]> GetDeals(BaseAccountRequest request, CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest($"accounts/{request.AccountId.Trim()}/deals", cancellationToken);
            DealsResponse dealResponse = new DealsResponse(response);
            return dealResponse.Deals;
        }

        /// <inheritdoc/>
        public async Task<Order[]> GetOrders(BaseAccountRequest request, CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest($"accounts/{request.AccountId.Trim()}/orders", cancellationToken);
            OrdersResponse ordersResponse = new OrdersResponse(response);
            return ordersResponse.Orders;
        }

        /// <inheritdoc/>
        public async Task<Instrument> GetInstrumentDescription(AccountTickerRequest request, CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest($"accounts/{request.AccountId.Trim()}/instruments/{request.Ticker.Trim()}", cancellationToken);
            InstrumentDescriptionResponse descriptionResponse = new InstrumentDescriptionResponse(response);
            return descriptionResponse.InstrumentData;
        }

        /// <summary>
        /// Get a list of all trading instruments available in StocksTrader in general 
        /// (excluding the rules and restrictions for a specific trading account).
        /// </summary>
        /// <param name="cancellationToken">Cancellation token</param>
        /// <see cref="InstrumentShort"/>
        /// <returns>Array of short descriptions</returns>
        public async Task<InstrumentShort[]> GetInstruments(CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest("instruments", cancellationToken);
            InstrumentsResponse instrumentsResponse = new InstrumentsResponse(response);
            return instrumentsResponse.Instruments;
        }

        /// <summary>
        /// Get the latest ask/bid/last quotes for an instrument.
        /// </summary>
        /// <param name="request"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public async Task<QuoteData> GetQuoteData(AccountTickerRequest request, CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest($"accounts/{request.AccountId.Trim()}/instruments/{request.Ticker.Trim()}/quote", cancellationToken);
            QuoteDataResponse quoteResponse = new QuoteDataResponse(response);
            return quoteResponse.Quote;
        }

        /// <summary>
        /// Terminate the user session (or perform the logout flow).
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public Task Logout(CancellationToken cancellationToken = default)
        {
            return SendRequest("logout", requestType: HttpRequestType.Post, cancellationToken: cancellationToken);
        }

        /// <summary>
        /// Delete unfulfilled Limit or Stop order.
        /// </summary>
        /// <param name="request"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public async Task CancelOrder(CancelOrderRequest request, CancellationToken cancellationToken = default)
        {
            await SendRequest($"accounts/{request.AccountId.Trim()}/orders/{request.Id.Trim()}",
                requestType: HttpRequestType.Delete,
               cancellationToken: cancellationToken);
        }

        /// <summary>
        /// Close an open deal.
        /// </summary>
        /// <param name="request"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public async Task CloseDeal(CloseDealRequest request, CancellationToken cancellationToken = default)
        {
            await SendRequest($"accounts/{request.AccountId.Trim()}/deals/{request.Id.Trim()}",
                requestType: HttpRequestType.Delete,
               cancellationToken: cancellationToken);
        }

        /// <summary>
        /// Set Stop Loss/Take Profit order for an open deal or edit them.
        /// </summary>
        /// <param name="request"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public async Task ModifyDeal(ModifyDealRequest request, CancellationToken cancellationToken = default)
        {
            await SendRequest($"accounts/{request.AccountId.Trim()}/deals/{request.Id.Trim()}",
               body: request.Body.ToFormData(), requestType: HttpRequestType.Put,
               cancellationToken: cancellationToken);
        }

        /// <summary>
        /// Modify Limit or Stop order parameters.
        /// </summary>
        /// <param name="request"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public async Task ModifyOrder(ModifyOrderRequest request, CancellationToken cancellationToken = default)
        {
            await SendRequest($"accounts/{request.AccountId.Trim()}/orders/{request.Id.Trim()}",
               body: request.Body.ToFormData(), requestType: HttpRequestType.Put,
               cancellationToken: cancellationToken);
        }

        /// <summary>
        /// Place a new order (Market, Limit, Stop).
        /// </summary>
        /// <param name="request"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public async Task<OrderId> PlaceOrder(PlaceOrderRequest request, CancellationToken cancellationToken = default)
        {
            BaseResponse response = await SendRequest($"accounts/{request.AccountId.Trim()}/orders",
                body: request.Body.ToFormData(), requestType: HttpRequestType.Post,
                cancellationToken: cancellationToken);

            OrderIdResponse orderIdResponse = new OrderIdResponse(response);
            return orderIdResponse.Order;
        }

        #endregion
    }
}
