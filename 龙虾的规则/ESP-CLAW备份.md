# schedules.json

[{
        "id":    "morning_news_8am",
        "enabled":    true,
        "kind":    "cron",
        "start_at_ms":    0,
        "end_at_ms":    0,
        "interval_ms":    0,
        "cron_expr":    "0 8 * * *",
        "event_type":    "schedule",
        "event_key":    "morning_news_8am",
        "source_channel":    "time",
        "chat_id":    "o9cq802R6LtOMFks_aAzk_8R3CfQ@im.wechat",
        "content_type":    "trigger",
        "session_policy":    "trigger",
        "text":    "请搜索今天的科技、AI、机器人、skills相关重要新闻，以及Steam史低价格游戏推荐。不要娱乐八卦。每条50-100字，整理5-8条。微信发送。格式：**分类名**：标题+一句话摘要。",
        "payload_json":    "{}",
        "max_runs":    0
    }]

---

# router_rules.json

[{
        "id":    "im_new_session",
        "description":    "Start a fresh persistent chat session when the user sends /new.",
        "enabled":    true,
        "consume_on_match":    true,
        "ack":    "{{event.source_channel}} new session started",
        "match":    {
            "event_type":    "message",
            "event_key":    "text",
            "content_type":    "text",
            "text":    "/new"
        },
        "actions":    [{
                "type":    "call_cap",
                "cap":    "roll_chat_session",
                "input":    {
                }
            }, {
                "type":    "send_message",
                "input":    {
                    "channel":    "{{event.source_channel}}",
                    "chat_id":    "{{event.chat_id}}",
                    "message":    "Started a new session."
                }
            }]
    }, {
        "id":    "im_any_message_working_reply",
        "description":    "Reply to any IM text message before other automation handling.",
        "enabled":    true,
        "consume_on_match":    false,
        "ack":    "{{event.source_channel}} working reply sent",
        "match":    {
            "event_type":    "message",
            "event_key":    "text",
            "content_type":    "text"
        },
        "actions":    [{
                "type":    "send_message",
                "input":    {
                    "channel":    "{{event.source_channel}}",
                    "chat_id":    "{{event.chat_id}}",
                    "message":    "🦞 ESP-Claw 即将回复..."
                }
            }]
    }, {
        "id":    "im_attachment_saved_reply",
        "description":    "Reply when an IM attachment is saved.",
        "enabled":    true,
        "ack":    "{{event.channel}} attachment saved",
        "match":    {
            "event_type":    "attachment_saved"
        },
        "actions":    [{
                "type":    "send_message",
                "input":    {
                    "channel":    "{{event.source_channel}}",
                    "chat_id":    "{{event.chat_id}}",
                    "message":    "文件保存 {{event.source_channel}}"
                }
            }]
    }, {
        "id":    "im_any_message_agent",
        "description":    "Route IM text messages to the agent.",
        "enabled":    true,
        "consume_on_match":    true,
        "ack":    "{{event.source_channel}} routed to agent",
        "match":    {
            "event_type":    "message",
            "event_key":    "text",
            "content_type":    "text"
        },
        "actions":    [{
                "type":    "run_agent",
                "input":    {
                    "target_channel":    "{{event.source_channel}}",
                    "session_policy":    "chat"
                }
            }]
    }, {
        "id":    "agent_stage_im_notify",
        "description":    "Deliver agent stage progress messages to the original IM chat.",
        "enabled":    false,
        "consume_on_match":    true,
        "ack":    "{{event.source_channel}} agent stage sent",
        "match":    {
            "source_cap":    "claw_core",
            "event_type":    "agent_stage",
            "content_type":    "text"
        },
        "actions":    [{
                "type":    "send_message",
                "input":    {
                    "event_type":    "{{event.event_type}}",
                    "channel":    "{{event.source_channel}}",
                    "chat_id":    "{{event.chat_id}}",
                    "message":    "{{event.text}}"
                }
            }]
    }, {
        "id":    "agent_out_message_send_message",
        "description":    "Deliver agent output messages to the original IM chat.",
        "enabled":    true,
        "consume_on_match":    true,
        "ack":    "{{event.source_channel}} agent output sent",
        "match":    {
            "source_cap":    "claw_core",
            "event_type":    "out_message",
            "content_type":    "text"
        },
        "actions":    [{
                "type":    "send_message",
                "input":    {
                    "channel":    "{{event.source_channel}}",
                    "chat_id":    "{{event.chat_id}}",
                    "message":    "{{event.text}}"
                }
            }]
    }, {
        "id":    "goodnight_qq_forward",
        "match":    {
            "event_type":    "schedule",
            "event_key":    "goodnight_greeting"
        },
        "actions":    [{
                "type":    "send_message",
                "input":    {
                    "channel":    "qq",
                    "chat_id":    "{{event.chat_id}}",
                    "message":    "{{event.text}}"
                }
            }]
    }, {
        "id":    "morning_news_agent",
        "match":    {
            "event_type":    "schedule",
            "event_key":    "morning_news_8am"
        },
        "actions":    [{
                "type":    "run_agent",
                "input":    {
                    "target_channel":    "wechat",
                    "session_policy":    "chat"
                }
            }]
    }]

---

# memory_index.json

{"version":3,"summaries":[{"summary_id":1,"label":"汕大","ref_count":1},{"summary_id":2,"label":"电子信息工程","ref_count":1},{"summary_id":3,"label":"大三","ref_count":1},{"summary_id":4,"label":"academic","ref_count":1},{"summary_id":5,"label":"exam_wee","ref_count":1},{"summary_id":6,"label":"internsh","ref_count":1},{"summary_id":7,"label":"career","ref_count":1},{"summary_id":8,"label":"zhuiyuan","ref_count":1},{"summary_id":9,"label":"微信","ref_count":1}],"keyword_index":{"汕头大学":["mem-1777903204-0006"],"电子信息工程":["mem-1777903204-0006"],"大三":["mem-1777903204-0006"],"考试周":["mem-1777912967-0009"],"第10周":["mem-1777912967-0009"],"academic_schedule":["mem-1777912967-0009"],"追远机器人":["mem-1777913120-0010"],"internship":["mem-1777913120-0010"],"实习":["mem-1777913120-0010"],"微信":["mem-1777954492-0002"],"发送方式":["mem-1777954492-0002"]},"last_compact_digest_size":74,"next_summary_id":10}

---

# memory_records.jsonl

{"id":"mem-1777903204-0006","source":"manual","content":"用户是汕头大学电子信息工程专业大三学生，住在桑浦山校区（汕大本部）。","tags":"汕大,电子信息工程,大三","keywords":"汕头大学,电子信息工程,大三","created_at":1777903204,"updated_at":1777903204,"access_count":0,"deleted":false,"summary_ids":[1,2,3],"summary_labels":[]}
{"id":"mem-1777912967-0009","source":"manual","content":"第17、18周是考试周，当前是第10周（2026年5月5日）。","tags":"academic_schedule,exam_weeks","keywords":"考试周,第10周,academic_schedule","created_at":1777912967,"updated_at":1777912967,"access_count":0,"deleted":false,"summary_ids":[4,5],"summary_labels":[]}
{"id":"mem-1777913120-0010","source":"manual","content":"考完第17-18周考试周后，用户计划去追远机器人实习。","tags":"internship,career,zhuiyuan_robot","keywords":"追远机器人,internship,实习","created_at":1777913120,"updated_at":1777913120,"access_count":0,"deleted":false,"summary_ids":[6,7,8],"summary_labels":[]}
{"id":"mem-1777954492-0002","source":"auto_llm","content":"发送方式改为微信","tags":"微信","keywords":"微信,发送方式","created_at":1777954492,"updated_at":1777954492,"access_count":0,"deleted":false,"summary_ids":[9]}

--- 

# memory.md

# Long-term Memory

## Summary Labels

- 汕大 (refs=1)
- 电子信息工程 (refs=1)
- 大三 (refs=1)
- academic (refs=1)
- exam_wee (refs=1)
- internsh (refs=1)
- career (refs=1)
- zhuiyuan (refs=1)
- 微信 (refs=1)

## Active Memories

- `mem-1777954492-0002` 发送方式改为微信
  time=2026-05-05 12:14:52 access=0 labels=["微信"]
- `mem-1777913120-0010` 考完第17-18周考试周后，用户计划去追远机器人实习。
  time=2026-05-05 00:45:20 access=0 labels=["internsh","career","zhuiyuan"]
- `mem-1777912967-0009` 第17、18周是考试周，当前是第10周（2026年5月5日）。
  time=2026-05-05 00:42:47 access=0 labels=["academic","exam_wee"]
- `mem-1777903204-0006` 用户是汕头大学电子信息工程专业大三学生，住在桑浦山校区（汕大本部）。
  time=2026-05-04 22:00:04 access=0 labels=["汕大","电子信息工程","大三"]

--- 
