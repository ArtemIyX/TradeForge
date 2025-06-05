using RoboforexAPI.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Accounts
{
    public class AccountStateResponse : BaseResponse
    {
        public AccountStateResponse() { }

        public AccountStateResponse(BaseResponse copy) : base(copy) { }

        public AccountState AccountStateData
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Object)
                {
                    return JsonSerializer.Deserialize<AccountState>(jsonElement.GetRawText()) ?? new AccountState();
                }
                return new AccountState();
            }
        }
    }
}
