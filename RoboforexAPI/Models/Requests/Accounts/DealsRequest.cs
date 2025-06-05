using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Requests.Accounts
{
    public class DealsRequest : BaseAccountRequest
    {
        public ulong? HistoryFrom { get; set; } = null;
        public ulong? HistoryTo { get; set; } = null;
        public int? Skip { get; set; } = null;
        public int? Limit { get; set; } = null;
    }
}
