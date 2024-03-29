                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 size] */
using CanFrameNameMapping = std::map<uint32_t, std::string>;  /* TODO: this is wasteful as fuck, rewrite it */
using CanFrameSizeMapping = std::map<uint32_t, uint8_t>;  /* TODO: this is wasteful as fuck, rewrite it */
using CanFrameDirectionMapping = std::map<uint32_t, char>;  /* TODO: this is wasteful as fuck, rewrite it */

using CanBitfieldInfo = std::vector<std::tuple<std::string, std::string, CanMap*>>;

class ICanEntryLoader
{
public:
    virtual ~ICanEntryLoader() { }

    virtual bool Load(const std::filesystem::path& path, std::vector<std::unique_ptr<CanTxEntry>>& e) = 0;
    virtual bool Save(const std::filesystem::path& path, std::vector<std::unique_ptr<CanTxEntry>>& e) = 0;
};

class XmlCanEntryLoader : public ICanEntryLoader
{
public:
    virtual ~XmlCanEntryLoader();

    bool Load(const std::filesystem::path& path, std::vector<std::unique_ptr<CanTxEntry>>& e) override;
    bool Save(const std::filesystem::path& path, std::vector<std::unique_ptr<CanTxEntry>>& e) override;
};

class ICanRxEntryLoader
{
public:
    virtual ~ICanRxEntryLoader() { }

    virtual bool Load(const std::filesystem::path& path, std::unordered_map<uint32_t, std::string>& e, std::unordered_map<uint32_t, uint8_t>& loglevels) = 0;
    virtual bool Save(const std::filesystem::path& path, std::unordered_map<uint32_t, std::string>& e, std::unordered_map<uint32_t, uint8_t>& loglevels) = 0;
};

class XmlCanRxEntryLoader : public ICanRxEntryLoader
{
public:
    virtual ~XmlCanRxEntryLoader();

    bool Load(const std::filesystem::path& path, std::unordered_map<uint32_t, std::string>& e, std::unordered_map<uint32_t, uint8_t>& loglevels) override;
    bool Save(const std::filesystem::path& path, std::unordered_map<uint32_t, std::string>& e, std::unordered_map<uint32_t, uint8_t>& loglevels) override;
};

class ICanMappingLoader
{
public:
    virtual ~ICanMappingLoader() { }

    virtual bool Load(const std::filesystem::path& path, CanMapping& mapping, CanFrameNameMapping& names, CanFrameSizeMapping& sizes, CanFrameDirectionMapping& directions) = 0;
    virtual bool Save(const std::filesystem::path& path, CanMapping& mapping, CanFrameNameMapping& names, CanFrameSizeMapping& sizes, CanFrameDirectionMapping& directions) = 0;
};

class XmlCanMappingLoader : public ICanMappingLoader
{
public:
    virtual ~XmlCanMappingLoader();

    bool Load(const std::filesystem::path& path, CanMapping& mapping, CanFrameNameMapping& names, CanFrameSizeMapping& sizes, CanFrameDirectionMapping& directions) override;
    bool Save(const std::filesystem::path& path, CanMapping& mapping, CanFrameNameMapping& names, CanFrameSizeMapping& sizes, CanFrameDirectionMapping& directions) override;

    static CanBitfieldType GetTypeFromString(const std::string_view& input);
    static const std::string_view GetStringFromType(CanBitfieldType type);

    static std::pair<int64_t, int64_t> GetMinMaxForType(CanBitfieldType type);

private:
    static inline std::map<CanBitfieldType, std::string> m_CanBitfieldTypeMap
    {
        {CBT_BOOL, "bool"},
        {CBT_UI8, "uint8_t"},
        {CBT_I8, "int8_t"},
        {CBT_UI16, "uint16_t"},
        {CBT_I16, "int16_t"},
        {CBT_UI32, "uint32_t"},
        {CBT_I32, "int32_t"},
        {CBT_UI64, "uint64_t"},
        {CBT_I64, "int64_t"},
        {CBT_FLOAT, "float"},
        {CBT_DOUBLE, "double"},
        {CBT_INVALID, "invalid"}
    };    
    
    static inline std::map<CanBitfieldType, std::pair<int64_t, int64_t>> m_CanTypeSizes  /* TODO: use int128_t for size from boost::multiprecision */
    {
        {CBT_BOOL, {0, 1}},
        {CBT_UI8, {std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}},
        {CBT_I8, {std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max()}},
        {CBT_UI16, {std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()}},
        {CBT_I16, {std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max()}},
        {CBT_UI32, {std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()}},
        {CBT_I32, {std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()}},
        //{CBT_UI64, "uint64_t"},
        {CBT_I64, {std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()}},
        //{CBT_FLOAT, "float"},
        //{CBT_DOUBLE, "double"},
        {CBT_INVALID, {0, 0}}
    };
};

class ICanSubscriber 
{
public:
    ICanSubscriber() = default;
    ~ICanSubscriber() = default;

    void RegisterObserver(ICanObserver* observer) 
    {
        m_Observers.push_back(observer);
    }

    void UnregisterObserver(ICanObserver* observer)
    {
        m_Observers.remove(observer);
    }

protected:
    void NotifyFrameOnBus(uint32_t frame_id, uint8_t* data, uint16_t size) const
    {
        for(auto observer : m_Observers)
            observer->OnFrameOnBus(frame_id, data, size);
    }    
    
    void NotifyIsoTpData(uint32_t frame_id, uint8_t* data, uint16_t size) const
    {
        for(auto observer : m_Observers)
            observer->OnIsoTpDataReceived(frame_id, data, size);
    }

    std::list<ICanObserver*> m_Observers;
};


class CanScriptHandler;
class CanEntryHandler : public ICanSubscriber
{
public:
    CanEntryHandler(ICanEntryLoader& loader, ICanRxEntryLoader& rx_loader, ICanMappingLoader& mapping_loader);
    ~CanEntryHandler();

    // !\brief Initialize entry handler
    void Init();

    // !\brief Load files (TX & RX List, Frame mapping)
    void LoadFiles();

    // !\brief Worker thread
    void WorkerThread(std::stop_token token);

    // !\brief Called when a can frame was sent
    void OnFrameSent(uint32_t frame_id, uint8_t data_len, uint8_t* data);

    // !\brief Called when a can frame was received
    void OnFrameReceived(uint32_t frame_id, uint8_t data_len, uint8_t* data);
    
    // !\brief Toggle automatic sending of all CAN frames which period isn't null
    // !\param toggle [in] Toggle auto send?
    void ToggleAutoSend(bool toggle);    
    
    // !\param Get CAN auto send state
    // !\return Is auto send toggled?
    bool IsAutoSend() { return auto_send; }

    // !\brief Toggle automatic CAN frame recording
    // !\param toggle [in] Toggle auto recording?
    void ToggleAutoRecord(bool toggle) { auto_recording = toggle; }

    // !\param Is auto recording enabled?
    // !\return Is auto recording enabled?
    bool IsAutoRecord() { return auto_recording; }

    // !\brief Toggle recording
    // !\param toggle [in] Toggle recording?
    // !\param is_puase [in] Is pause?
    void ToggleRecording(bool toggle, bool is_puase);

    // !\brief Clear recorded frames
    void ClearRecording();

    // !\brief Send Data Frame over CAN BUS
    // !\param frame_id [in] CAN Frame ID
    // !\param data [in] Data to send
    // !\param size [in] Data size
    void SendDataFrame(uint32_t frame_id, uint8_t* data, uint16_t size);

    // !\brief Send ISO-TP frame over CAN BUS
    // !\param frame_id [in] CAN Frame ID
    // !\param data [in] Data to send
    // !\param size [in] Data size
    void SendIsoTpFrame(uint32_t frame_id, uint8_t* data, uint16_t size);

    // !\brief Load TX list from a file
    // !\param path [in] File path to load
    // !\return Is load was successfull?
    bool LoadTxList(std::filesystem::path& path);

    // !\brief Save TX list to a file
    // !\param path [in] File path to save
    bool SaveTxList(std::filesystem::path& path);

    // !\brief Load RX list from a file
    // !\param path [in] File path to load
    // !\return Is load was successfull?
    bool LoadRxList(std::filesystem::path& path);

    // !\brief Save RX list to a file
    // !\param path [in] File path to save
    bool SaveRxList(std::filesystem::path& path);
    
    // !\brief Load CAN mapping from a file
    // !\param path [in] File path to save
    bool LoadMapping(std::filesystem::path& path);

    // !\brief Save CAN mapping to a file
    // !\param path [in] File path to save
    bool SaveMapping(std::filesystem::path& path);

    // !\brief Save recorded data to file
    // !\param path [in] File path to save
    bool SaveRecordingToFile(std::filesystem::path& path);

    // !\brief Get recording level
    uint8_t GetRecordingLogLevel() { return m_RecodingLogLevel; }

    // !\brief Set recording level
    // !\param log_level [in] Recording level
    void SetRecordingLogLevel(uint8_t log_level) { m_RecodingLogLevel = log_level; }

    // !\brief Get favourite level
    uint8_t GetFavouriteLevel() { return m_DefaultFavouriteLevel; }

    // !\brief Set favourite level
    // !\param favourite_level [in] Favourite level
    void SetFavouriteLevel(uint8_t favourite_level) { m_DefaultFavouriteLevel = favourite_level; }

    // !\brief Get default ECU ID
    uint32_t GetDefaultEcuId() { return m_DefaultEcuId; }

    // !\brief Set default ECU ID
    // !\param ecu_id [in] Default ECU ID
    void SetDefaultEcuId(uint32_t ecu_id) { m_DefaultEcuId = ecu_id; link.send_arbitration_id = m_DefaultEcuId; }

    // !\brief Get log records for given frame
    // !\param frame_id [in] CAN Frame ID
    // !\param is_rx [in] Is RX?
    // !\param log [out] Vector of rows
    void GenerateLogForFrame(uint32_t frame_id, bool is_rx, std::vector<std::string>& log);

    // !\brief Set ISO-TP response frame
    // !\param frame_id [in] CAN Frame ID
    void SetIsoTpResponseFrame(uint32_t frame_id) { m_IsoTpResponseId = frame_id; }

    // !\brief Get ISO-TP response Frame ID
    uint32_t GetIsoTpResponseFrameId() { return m_IsoTpResponseId; };

    // !\brief Return TX Frame count
    // !\return TX Frame count
    uint64_t GetTxFrameCount() { return tx_frame_cnt; }

    // !\brief Return RX Frame count
    // !\return RX Frame count
    uint64_t GetRxFrameCount() { return rx_frame_cnt; }

    // !\brief Get map for frame id (in string format)
    // !\param frame_id [in] CAN Frame ID
    // !\param is_rx [in] Is RX?
    CanBitfieldInfo GetMapForFrameId(uint32_t frame_id, bool is_rx);

    // !\brief Apply editing on CAN frame bitfields
    // !\param frame_id [in] CAN Frame ID
    // !\param new_data [in] Vector of strings of new data
    void ApplyEditingOnFrameId(uint32_t frame_id, std::vector<std::string> new_data);

    uint32_t FindFrameIdOnMapByName(const std::string& name);

    CanMapping& GetMapping() { return m_mapping; }

    std::vector<std::string>& GetUdsRawBuffer() { return m_UdsFrames; }

    uint32_t GetElapsedTimeSinceLastUdsFrame();

    std::chrono::steady_clock::time_point GetStartTime() { return start_time; }

    // !\brief Vector of CAN TX entries
    std::vector<std::unique_ptr<CanTxEntry>> entries;

    // !\brief Vector of CAN RX entries
    std::unordered_map<uint32_t, std::unique_ptr<CanRxData>> m_rxData;  /* [frame_id] = rxData */

    // !\brief Frame ID comment
    std::unordered_map<uint32_t, std::string> rx_entry_comment;  /* [frame_id] = comment msg */

    // !\brief Frame ID log levels
    std::unordered_map<uint32_t, uint8_t> m_RxLogLevels;

    // !\brief CAN Log entries (both TX & RX)
    std::vector<std::unique_ptr<CanLogEntry>> m_LogEntries;

    // !\brief Path to default TX list
    std::filesystem::path default_tx_list;
    
    // !\brief Path to default RX list
    std::filesystem::path default_rx_list;    
    
    // !\brief Path to default CAN mapping
    std::filesystem::path default_mapping;

    // !\brief Mutex for entry handler
    std::mutex m;

    // !\brief Can frame name mapping [frame_id] = name
    CanFrameNameMapping m_frame_name_mapping;

    // !\brief Can frame size mapping [frame_id] = name
    CanFrameSizeMapping m_frame_size_mapping;    
    
    // !\brief Can frame direction mapping [frame_id] = name
    CanFrameDirectionMapping m_frame_direction_mapping;

    // !\brief Assigns new TX buffer to TX entry
    void AssignNewBufferToTxEntry(uint32_t frame_id, uint8_t* buffer, size_t size);

private:
    // !\brief Handle bit reading of a frame
    template <typename T> void HandleBitReading(uint32_t frame_id, bool is_rx, std::unique_ptr<CanMap>& m, size_t offset, CanBitfieldInfo& info);

    // !\brief Handle bit writing of a frame
    template <typename T> void HandleBitWriting(uint32_t frame_id, uint8_t& pos, uint8_t offset, uint8_t size, uint8_t* byte_array, std::vector<std::string>& new_data);

    // !\brief Find CAN TX Entry by Frame ID
    std::optional<std::reference_wrapper<CanTxEntry>> FindTxCanEntryByFrame(uint32_t frame_id);

    // !\brief Reference to CAN TX entry loader
    ICanEntryLoader& m_CanEntryLoader;

    // !\brief Reference to CAN RX entry loader
    ICanRxEntryLoader& m_CanRxEntryLoader;    
    
    // !\brief Reference to CAN mapping loader
    ICanMappingLoader& m_CanMappingLoader;

    // !\brief Sending every can frame automatically at startup which period is not null? 
    bool auto_send = false;

    // !\brief Start recording automatically?
    bool auto_recording = false;

    // !\brief Is recording on?
    bool is_recoding = false;

    // !\brief TX Frame count
    uint64_t tx_frame_cnt = 0;

    // !\brief RX Frame count
    uint64_t rx_frame_cnt = 0;

    // !\brief Exit worker thread?
    //std::stop_source stop_source;

    // !\brief Worker thread
    std::unique_ptr<std::jthread> m_worker;

    // !\brief Conditional variable for main thread exiting
    std::condition_variable_any m_cv;

    // !\brief Starting time
    std::chrono::steady_clock::time_point start_time;

    // !\brief CAN mapping container
    CanMapping m_mapping;

    // !\brief Recording log level
    uint8_t m_RecodingLogLevel = 1;

    // !\brief Favourite level
    // !\details CAN entries (both TX & RX) which level is below this level, won't show on TX and RX list
    uint8_t m_DefaultFavouriteLevel = 1;

    // !\brief Default ECU ID
    uint32_t m_DefaultEcuId = 0x8AB;
public:
    // !\brief ISO-TP Link
    IsoTpLink link;

    // !\brief ISO-TP Receiving buffer
    uint8_t m_Isotp_Sendbuf[MAX_ISOTP_FRAME_LEN];

    // !\brief ISO-TP Transmit buffer
    uint8_t m_Isotp_Recvbuf[MAX_ISOTP_FRAME_LEN];

    // !\brief Vector of received ISO-TP frames (Usally UDS). 
    // !\note This buffer growing always, user has to clear it
    std::vector<std::string> m_UdsFrames;  /* TODO: add mutex for this */
    
    // !\brief Buffer for data received over ISO-TP (Usally UDS)
    uint8_t m_uds_recv_data[4096] = {};

    // !\brief ISO-TP Response Frame ID
    uint32_t m_IsoTpResponseId = 0x7DA;

    std::chrono::steady_clock::time_point last_uds_frame_received;
};