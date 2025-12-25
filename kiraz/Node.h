#ifndef KIRAZ_NODE_H
#define KIRAZ_NODE_H

#include <iostream>
#include <memory>
#include <string>

#include <fmt/format.h>

// FF makrosunu buraya taşıyoruz ki her yerde kullanılabilsin
#define FF fmt::format

// Forward declarations (İleri bildirimler)
// Compiler.h içindeki sınıfları burada tanıtıyoruz
namespace kiraz {
    class SymbolTable;
    class WasmContext;
}

class Node : public std::enable_shared_from_this<Node> {
public:
    using Ptr = std::shared_ptr<Node>;
    using SymTabEntry = std::pair<std::string, Ptr>;

    Node();
    virtual ~Node();

    // ID İşlemleri
    void set_id(const std::string &id) { m_id = id; }
    const std::string &get_id() const { return m_id; }

    // Konum Bilgileri
    void set_line(int line) { m_line = line; }
    int get_line() const { return m_line; }
    void set_col(int col) { m_col = col; }
    int get_col() const { return m_col; }

    // Hata Yönetimi
    void set_error(const std::string &error) { m_error = error; }
    const std::string &get_error() const { return m_error; }

    // Semantik Analiz
    // SymbolTable artık kiraz namespace'i altında
    virtual Ptr compute_stmt_type(kiraz::SymbolTable &st);
    virtual Ptr add_to_symtab_ordered(kiraz::SymbolTable &st);
    virtual Ptr add_to_symtab_forward(kiraz::SymbolTable &st);
    
    // Semantik Helperlar
    void set_stmt_type(Ptr type) { m_stmt_type = type; }
    Ptr get_stmt_type() const { return m_stmt_type; }
    virtual SymTabEntry get_subsymbol(Ptr name) const;

    // Kod Üretimi (WASM)
    // WasmContext artık kiraz namespace'i altında
    virtual Ptr gen_wat(kiraz::WasmContext &ctx);

    // Sanal Metotlar (Type Checks)
    virtual std::string as_string() const = 0;
    virtual bool is_stmt_list() const { return false; }
    virtual bool is_funcarg_list() const { return false; }
    virtual bool is_func() const { return false; }
    virtual bool is_class() const { return false; }
    virtual bool is_import() const { return false; }
    virtual bool is_return() const { return false; }
    virtual bool is_if() const { return false; }
    virtual bool is_while() const { return false; }
    virtual bool is_dot() const { return false; }
    virtual bool is_call() const { return false; }
    virtual bool is_assign() const { return false; }

    // Static Root Yönetimi
    static void set_root(Ptr root);
    static Ptr get_root();
    static Ptr pop_root();
    static void reset_root();
    static Ptr current_root(); // Testler için eklendi
    static Ptr get_root_before(); // Testler için

    // Scope Yönetimi
    void set_cur_symtab(std::shared_ptr<void> st) { m_cur_symtab = st; }
    std::shared_ptr<void> get_cur_symtab() const { return m_cur_symtab; }

protected:
    std::string m_id;
    int m_line = 0;
    int m_col = 0;
    std::string m_error;
    Ptr m_stmt_type;
    std::shared_ptr<void> m_cur_symtab;

private:
    static Ptr s_root;
    static Ptr s_root_before;
};

// Loglama operatörü
inline std::ostream &operator<<(std::ostream &os, const Node &node) {
    os << node.as_string();
    return os;
}

#endif
