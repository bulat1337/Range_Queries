#ifndef RB_TREE_H
#define RB_TREE_H

#include <cstdlib>

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <stack>

#include "log.h"

namespace RB
{

namespace detail
{

namespace html_colors
{
static constexpr const char *blue_gray_ = "#48565D";
static constexpr const char *coral_pink_ = "#F08080";
static constexpr const char *navy_blue_ = "#0B0042";
static constexpr const char *black_ = "#1B1B1C";
static constexpr const char *red_ = "#820007";
}; // namespace html_colors

}; // namespace detail

template <typename KeyT> class Tree
{
  private:
    /* -----~ Usings ~----- */
    struct Node;
    using NodePtr = std::shared_ptr<Node>;

    /* -----~ Node ~----- */
    struct Node
    {
        KeyT value;

        NodePtr right{};
        NodePtr left{};
        NodePtr parent{};

        bool is_red = true;

        Node(const KeyT &_value)
            : value(_value)
        {}

        Node(const KeyT &_value, NodePtr _parent, bool _is_red = true)
            : value(_value)
            , parent(_parent)
            , is_red(_is_red)
        {}

        NodePtr get_grandp() const
        {
            if (parent != nullptr)
                return parent->parent;

            return nullptr;
        }

        NodePtr get_unc() const
        {
            NodePtr grandparent = get_grandp();

            if (grandparent == nullptr)
                return nullptr;

            return grandparent->right == parent ? grandparent->left
                                                : grandparent->right;
        }

        bool is_left_child() const { return parent->left.get() == this; }

        bool is_right_child() const { return !is_left_child(); }

        bool is_triangle() const
        {
            return (is_left_child() && parent->is_right_child()) ||
                   (is_right_child() && parent->is_left_child());
        }
    };

    /* -----~ Iterator ~----- */

    static NodePtr sub_begin(NodePtr node)
    {
        while (node && node->left)
            node = node->left;
        return node;
    }

    static NodePtr sub_end(NodePtr node)
    {
        while (node && node->right)
            node = node->right;
        return node;
    }

    static NodePtr next(NodePtr node)
    {
        if (node->right)
            return sub_begin(node->right);

        NodePtr parent = node->parent;

        while (parent && node == parent->right)
        {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

    static NodePtr prev(NodePtr node)
    {
        if (node->left)
            return sub_end(node->left);

        NodePtr parent = node->parent;
        while (parent && node == parent->left)
        {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

  public:
    class iterator
    {
      public:
        using value_type = KeyT;
        using difference_type = std::ptrdiff_t;
        using reference = KeyT &;
        using pointer = NodePtr;
        using iterator_category = std::bidirectional_iterator_tag;

      public:
        pointer node_;

        iterator(pointer node = nullptr)
            : node_(node)
        {}

        iterator(const iterator &other)
            : node_(other.node_)
        {}

        iterator &operator=(const iterator &other)
        {
            node_ = other.node_;
            return *this;
        }

        void swap(iterator &other) { std::swap(node_, other.node_); }

        KeyT &operator*() const { return node_->value; }

        iterator &operator++()
        {
            if (node_)
                LOG("incrementing iterator of value {}\n", node_->value);
            else
                MSG("incrementing null iterator\n");

            node_ = Tree::next(node_);
            return *this;
        }

        iterator operator++(KeyT)
        {
            if (node_)
                LOG("postincrementing iterator of value {}\n", node_->value);
            else
                MSG("postincrementing null iterator\n");

            iterator tmp(*this);
            node_ = Tree::next(node_);
            return tmp;
        }

        iterator &operator--()
        {
            if (node_)
                LOG("decrementing iterator of value {}\n", node_->value);
            else
                MSG("decrementing null iterator\n");

            node_ = Tree::prev(node_);
            return *this;
        }

        iterator operator--(KeyT)
        {
            if (node_)
                LOG("postdecrementing iterator of value {}\n", node_->value);
            else
                MSG("postdecrementing null iterator\n");

            iterator tmp(*this);
            node_ = Tree::prev(node_);
            return tmp;
        }

        bool operator==(const iterator &other) const
        {
            MSG("operator== called\n");
            return node_ == other.node_;
        }

        bool operator==(const std::nullptr_t &other) const
        {
            MSG("operator== for nullptr called\n");
            return node_ == other;
        }

        bool operator!=(const iterator &other) const
        {
            if (this->node_ && other.node_)
                LOG("operator!= called for {} and {}\n", this->node_->value,
                    other.node_->value);
            else
                MSG("operator!= called for null iterator\n");

            return node_ != other.node_;
        }

        bool operator!=(const std::nullptr_t &other) const
        {
            MSG("operator== for nullptr called\n");
            return node_ != other;
        }
    };

    void swap(iterator &lhs, iterator &rhs) { lhs.swap(rhs); }

    iterator beign() { return iterator(sub_begin(root_)); }
    iterator end() { return iterator(nullptr); }

  private:
    /* -----~ members ~----- */
    NodePtr root_ = nullptr;

    /* -----~ private member-functions ~----- */
    void rotate_left(NodePtr node)
    {
        NodePtr pivot = node->right;

        pivot->parent = node->parent;

        if (node->parent != nullptr)
        {
            if (node->parent->left == node)
                node->parent->left = pivot;
            else
                node->parent->right = pivot;
        }
        else
            root_ = pivot;

        node->right = pivot->left;
        if (pivot->left != nullptr)
            pivot->left->parent = node;

        node->parent = std::move(pivot);
        node->parent->left = std::move(node);
    }

    void rotate_right(NodePtr node)
    {
        NodePtr pivot = node->left;

        pivot->parent = node->parent;
        if (node->parent != nullptr)
        {
            if (node->parent->left == node)
                node->parent->left = pivot;
            else
                node->parent->right = pivot;
        }
        else
            root_ = pivot;

        node->left = pivot->right;
        if (pivot->right != nullptr)
            pivot->right->parent = node;

        node->parent = std::move(pivot);
        node->parent->right = std::move(node);
    }

    void subtree_insert(NodePtr sub_root, const KeyT &value)
    {
        if (value > sub_root->value)
        {
            if (sub_root->right == nullptr)
            {
                NodePtr inserted_node = std::make_shared<Node>(value, sub_root);
                sub_root->right = inserted_node;

                fix_violation(inserted_node);

                return;
            }

            subtree_insert(sub_root->right, value);
        }
        else if (value < sub_root->value)
        {
            if (sub_root->left == nullptr)
            {
                NodePtr inserted_node = std::make_shared<Node>(value, sub_root);
                sub_root->left = inserted_node;

                fix_violation(inserted_node);

                return;
            }

            subtree_insert(sub_root->left, value);
        }

        return;
    }

    void handle_black_unc(NodePtr cur_node)
    {
        NodePtr parent = cur_node->parent;
        NodePtr grandparent = cur_node->get_grandp();

        if (cur_node->is_left_child() && parent->is_right_child())
        {
            rotate_right(parent);
            cur_node = cur_node->right;
        }
        else if (cur_node->is_right_child() && parent->is_left_child())
        {
            rotate_left(parent);
            cur_node = cur_node->left;
        }

        parent = cur_node->parent;
        grandparent = cur_node->get_grandp();

        if (cur_node->is_left_child())
            rotate_right(grandparent);
        else
            rotate_left(grandparent);

        paint_black(parent);
        paint_red(grandparent);
    }

    void fix_violation(NodePtr cur_node)
    {
        while (cur_node != root_ && cur_node->parent->is_red)
        {
            NodePtr parent = cur_node->parent;
            NodePtr uncle = cur_node->get_unc();

            if (uncle && uncle->is_red)
            {
                paint_black(parent);
                paint_black(uncle);

                NodePtr grandparent = cur_node->get_grandp();

                paint_red(grandparent);
                cur_node = grandparent;
            }
            else
            {
                handle_black_unc(cur_node);
                break;
            }
        }
        paint_black(root_);
    }

    void paint_black(NodePtr node) const
    {
        if (node != nullptr)
            node->is_red = false;
    }

    void paint_red(NodePtr node) const { node->is_red = true; }

    NodePtr create_based_on(const NodePtr reference, NodePtr parent)
    {
        if (reference == nullptr)
            return nullptr;

        LOG("Copying {}\n", reference->value);
        NodePtr created =
            std::make_shared<Node>(reference->value, parent, reference->is_red);

        created->left = create_based_on(reference->left, created);
        created->right = create_based_on(reference->right, created);

        return created;
    }

    /* -----~ graphviz dump ~----- */
    void dump_regular_nodes(NodePtr node, std::ostream &dump) const
    {
        if (node == nullptr)
            return;

        dump << "\t" << reinterpret_cast<size_t>(node.get())
             << "[shape = Mrecord, fillcolor = \""
             << (node->is_red ? detail::html_colors::red_
                              : detail::html_colors::black_)
             << "\", label =  \"{" << node << " | val: " << node->value
             << " | {L: " << node->left.get() << " R: " << node->right.get()
             << "}}"
             << "\" ];\n";

        dump_regular_nodes(node->left, dump);

        dump_regular_nodes(node->right, dump);
    }

    void dump_connections(NodePtr node, std::ostream &dump) const
    {
        if (node == nullptr)
            return;

        if (node->left != nullptr)
        {
            dump << reinterpret_cast<size_t>(node.get()) << " -> "
                 << reinterpret_cast<size_t>(node->left.get()) << '\n';

            dump_connections(node->left, dump);
        }

        if (node->right != nullptr)
        {
            dump << reinterpret_cast<size_t>(node.get()) << " -> "
                 << reinterpret_cast<size_t>(node->right.get()) << '\n';

            dump_connections(node->right, dump);
        }
    }

  public:
    Tree() = default;

    Tree(const Tree &other)
    {
        MSG("Copy constructor called\n");

        if (!other.root_)
            return;

        struct NodeContext
        {
            NodePtr original;
            NodePtr copy;
        };

        std::stack<NodeContext> stack;

        root_ = std::make_shared<Node>(other.root_->value, nullptr,
                                       other.root_->is_red);
        stack.push({other.root_, root_});

        while (!stack.empty())
        {
            NodeContext ctxt = stack.top();
            stack.pop();

            if (ctxt.original->left)
            {
                ctxt.copy->left = std::make_shared<Node>(
                    ctxt.original->left->value, ctxt.copy,
                    ctxt.original->left->is_red);

                stack.push({ctxt.original->left, ctxt.copy->left});
            }

            if (ctxt.original->right)
            {
                ctxt.copy->right = std::make_shared<Node>(
                    ctxt.original->right->value, ctxt.copy,
                    ctxt.original->right->is_red);

                stack.push({ctxt.original->right, ctxt.copy->right});
            }
        }
    }

    Tree &operator=(const Tree &other)
    {
        MSG("Copy assignment called\n");

        if (this == &other)
            return *this;

        // free_data();

        root_ = create_based_on(other.root_, nullptr);

        return *this;
    }

    Tree(Tree &&other) noexcept
        : root_(std::move(other.root_))
    // , data_(std::move(other.data_))
    {
        MSG("Move constructor called\n");
        // other.data_.clear();
        other.root_ = nullptr;
    }

    Tree &operator=(Tree &&other) noexcept
    {
        MSG("Move assignment called\n");

        if (this == &other)
            return *this;

        root_ = std::move(other.root_);

        return *this;
    }

    void insert(const KeyT &value)
    {
        LOG("Inserting {}\n", value);

        if (root_ == nullptr)
        {
            root_ = std::make_shared<Node>(value);
            paint_black(root_);
        }
        else
            subtree_insert(root_, value);
    }

    void dump() const
    {
        std::string file_name = "tree_dump";
        std::ofstream dump(file_name + ".dot");

        dump << "digraph BinaryTree {\n"
                "bgcolor = \""
             << detail::html_colors::blue_gray_
             << "\";\n"
                "edge[minlen = 3, penwidth = 3; color = \"black\"];\n"
                "node[shape = \"rectangle\", style = \"rounded, filled\",\n"
                "\tfillcolor = \""
             << detail::html_colors::coral_pink_
             << "\",\n"
                "\tfontsize = 30,\n"
                "\theight = 3,\n"
                "\tpenwidth = 5, color = \"white\", fontcolor = \"white\"];\n";

        dump << "{rank = min;\n"
                "\tlist_manager [shape = Mrecord, fillcolor = \""
             << detail::html_colors::navy_blue_
             << "\", "
                "label = \"{ROOT: "
             << root_
             << "}\"];\n"
                "}\n";

        if (root_ != nullptr)
            dump_regular_nodes(root_, dump);

        dump_connections(root_, dump);

        dump << "}";
    }

    iterator lower_bound(const KeyT &key) const
    {
        NodePtr cur_node = root_;
        NodePtr answer{};

        if (!cur_node)
            return iterator{};

        while (true)
        {
            if (key < cur_node->value)
            {
                answer = cur_node;
                if (cur_node->left == nullptr)
                    return iterator(answer);
                cur_node = cur_node->left;
            }
            else if (key > cur_node->value)
            {
                if (cur_node->right == nullptr)
                    return iterator(answer);
                cur_node = cur_node->right;
            }
            else
                return iterator(cur_node);
        }

        return iterator(answer);
    }

    iterator upper_bound(const KeyT &key) const
    {
        NodePtr cur_node = root_;
        NodePtr answer = nullptr;

        if (!cur_node)
            return iterator{};

        while (true)
        {
            if (key < cur_node->value)
            {
                answer = cur_node;
                if (cur_node->left == nullptr)
                    return iterator(answer);
                cur_node = cur_node->left;
            }
            else if (key >= cur_node->value)
            {
                if (cur_node->right == nullptr)
                    return iterator(answer);
                cur_node = cur_node->right;
            }
        }

        return iterator(answer);
    }
};

}; // namespace RB

#endif // RB_TREE_H
