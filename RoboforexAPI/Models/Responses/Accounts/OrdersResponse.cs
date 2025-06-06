﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Accounts
{
    public class OrdersResponse : BaseResponse
    {
        public OrdersResponse() { }

        public OrdersResponse(BaseResponse copy) : base(copy) { }

        public Entities.Order[] Orders
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<Entities.Order[]>(jsonElement.GetRawText()) ?? [];
                }
                return [];
            }
        }
    }
}
