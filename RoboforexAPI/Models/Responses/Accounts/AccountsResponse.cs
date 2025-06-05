using RoboforexAPI.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Accounts
{
    public class AccountsResponse : BaseResponse
    {
        public AccountsResponse() { }

        public AccountsResponse(BaseResponse copy) : base(copy) { }

        public Entities.Account[] Accounts
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<Entities.Account[]>(jsonElement.GetRawText()) ?? [];
                }
                return [];
            }
        }
    }
}
