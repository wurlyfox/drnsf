//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
//
// See the AUTHORS.md file for more details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

#include <map>
#include <vector>
#include "transact.hh"

#define DEFINE_APROP(name) \
	::res::prop<decltype(m_##name)> p_##name = { *this, m_##name }; \
	const decltype(m_##name) &get_##name() const \
	{ \
		return p_##name.get(); \
	} \
	void set_##name(TRANSACT,decltype(m_##name) value) \
	{ \
		p_##name.set(TS,std::move(value)); \
	}

namespace res {

class asset;

class atom {
	friend class asset;

	struct nucleus;

private:
	nucleus *m_nuc;

	explicit atom(nucleus *nuc) noexcept;

	asset *&get_internal_asset_ptr() const;

public:
	static atom make_root();

	atom() noexcept;
	atom(std::nullptr_t) noexcept;
	atom(const atom &other) noexcept;
	atom(atom &&other) noexcept;
	~atom() noexcept;

	atom &operator =(atom other);

	bool operator ==(const atom &other) const noexcept;
	bool operator ==(std::nullptr_t) const noexcept;
	bool operator !=(const atom &other) const noexcept;
	bool operator !=(std::nullptr_t) const noexcept;

	explicit operator bool() const noexcept;
	bool operator !() const noexcept;

	atom operator /(const char *s) const;
	atom operator /(const std::string &s) const;

	asset *get() const;

	template <typename T>
	T *get_as() const
	{
		return dynamic_cast<T*>(get());
	}

	template <typename T>
	bool is_a() const
	{
		return (get_as<T>() != nullptr);
	}

	std::string name() const;
	std::string full_path() const;

	std::vector<atom> get_children() const;
	std::vector<atom> get_children_recursive() const;

	std::vector<atom> get_asset_names() const
	{
		return get_children_recursive();
	}

	friend std::string to_string(const atom &atom)
	{
		return atom.full_path();
	}
};

class asset : private util::nocopy {
private:
	atom m_name;

protected:
	explicit asset() = default;

public:
	virtual ~asset() = default;

	void assert_alive() const;

	template <typename T>
	static void create(TRANSACT,atom name)
	{
		if (!name)
			throw 0; // FIXME

		if (name.get())
			throw 0; // FIXME

		auto t = new T;
		TS.set(t->m_name,name);
		TS.set(name.get_internal_asset_ptr(),t);
		// FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
		// at what point do we free t now? used to be a unique_ptr...
		// FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
	}

	void rename(TRANSACT,atom name);
	void destroy(TRANSACT);

	const atom &get_name() const;

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
	}
};

class project : private util::nocopy {
private:
	atom m_root;
	std::list<std::unique_ptr<asset>> m_assets;
	transact::nexus m_transact;

public:
	project() :
		m_root(atom::make_root()) {}

	const atom &get_asset_root()
	{
		return m_root;
	}

	const decltype(m_assets) &get_asset_list()
	{
		return m_assets;
	}

	transact::nexus &get_transact()
	{
		return m_transact;
	}
};

template <typename T>
class prop : private util::nocopy {
private:
	asset &m_owner;
	T &m_value;

public:
	prop(asset &owner,T &value) :
		m_owner(owner),
		m_value(value) {}

	const T &get() const
	{
		m_owner.assert_alive();
		return m_value;
	}

	void set(TRANSACT,T value)
	{
		m_owner.assert_alive();
		TS.set(m_value,std::move(value));
	}
};

template <typename T>
class ref : public atom {
public:
	ref() = default;

	ref(const atom &other) :
		atom(other) {}

	ref(atom &&other) :
		atom(std::move(other)) {}

	void create(TRANSACT) const
	{
		res::asset::create<T>(TS,*this);
	}

	bool ok() const
	{
		return (static_cast<bool>(*this) && is_a<T>());
	}

	T *operator ->() const
	{
		return &operator *();
	}

	T &operator *() const
	{
		auto result = get_as<T>();
		if (!result) {
			throw 0;//FIXME
		}
		return *result;
	}
};

using anyref = ref<asset>;

}
