using RoboforexAPI.Models.Entities;
using System.Text.Json;

namespace RoboforexAPI.Models.Responses.Trading
{
    public class OrderIdResponse : BaseResponse
    {
        public Entities.OrderId Order
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Object)
                {
                    return JsonSerializer.Deserialize<OrderId>(jsonElement.GetRawText()) ?? new OrderId();
                }
                return new OrderId();
            }
        }
    }
}
