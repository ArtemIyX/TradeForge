using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Requests.Trading
{
    public class TradingIdRequest : BaseAccountRequest
    {
        public string Id { get; set; } = string.Empty;
    }
}
