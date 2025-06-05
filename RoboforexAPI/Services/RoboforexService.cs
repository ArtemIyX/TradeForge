using Microsoft.Extensions.Logging;
using RoboforexAPI.Models.Entities;
using RoboforexAPI.Models.Interfaces;
using RoboforexAPI.Models.Requests;
using RoboforexAPI.Models.Requests.Trading;
using RoboforexAPI.Models.Responses;
using RoboforexAPI.Models.Responses.Instruments;
using System.Net.Http.Json;
using System.Text.Json;

namespace RoboforexAPI.Services
{
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

        protected void AddAuth(HttpRequestMessage request)
        {
            if (!string.IsNullOrEmpty(ApiToken))
            {
                request.Headers.Add("Authorization", ApiToken);
            }
        }

        protected void AddContent(HttpRequestMessage request, string? content)
        {
            if (!string.IsNullOrEmpty(content))
            {
                request.Content = new StringContent(content, System.Text.Encoding.UTF8, "application/json");
            }
        }

        protected async Task<BaseResponse> FetchResponse(HttpResponseMessage response,
            CancellationToken cancellationToken = default)
        {
            if (response.StatusCode == System.Net.HttpStatusCode.Unauthorized)
            {
                throw new HttpRequestException("Unauthorized: Invalid or missing authentication credentials.",
                    null, System.Net.HttpStatusCode.Unauthorized);
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

        public enum HttpRequestType
        {
            Post,
            Put,
            Delete
        }

        protected async Task<BaseResponse> SendRequest(string query, 
            string? body = null, 
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
                AddContent(requestMessage, body);

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
        public Task CancelOrder(CancelOrderRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task CloseDeal(CloseDealRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<Account[]> GetAccounts(CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<AccountState> GetAccountState(BaseAccountRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<Deal[]> GetDeals(BaseAccountRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<Instrument> GetInstrumentDescription(AccountTickerRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public async Task<InstrumentShort[]> GetInstruments(CancellationToken cancellationToken = default)
        {
            BaseResponse response = await GetRequest("instruments", cancellationToken);
            InstrumentsResponse instrumentsResponse = new InstrumentsResponse(response);
            return instrumentsResponse.Instruments;
        }

        public Task<Order[]> GetOrders(BaseAccountRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task<QuoteData> GetQuoteData(AccountTickerRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task Logout(CancellationToken cancellationToken = default)
        {
            return SendRequest("logout", requestType: HttpRequestType.Post, cancellationToken: cancellationToken);
        }

        public Task ModifyDeal(ModifyDealRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task ModifyOrder(ModifyOrderRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        public Task PlaceOrder(PlaceOrderRequest request, CancellationToken cancellationToken = default)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
