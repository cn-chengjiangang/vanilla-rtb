/*
 * File:   campaign_entity.hpp
 * Author: Vladimir Venediktov vvenedict@gmail.com
 * Copyright (c) 2016-2018 Venediktes Gruppe, LLC
 *
 * Created on March 12, 2017, 04:04 PM
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/


#ifndef __IPC_DATA_CAMPAIGN_ENTITY_HPP__
#define __IPC_DATA_CAMPAIGN_ENTITY_HPP__

#include "base_entity.hpp"
#include <string>
#include <cstdint>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace ipc { namespace data {
    
    template <typename Alloc>
    struct campaign_entity : base_entity<Alloc>
    {
        using char_string = typename base_entity<Alloc>::char_string;
        using base_type   = base_entity<Alloc>; 
      
        //for tagging in multi_index_container
        struct campaign_id_tag {}; // search on campaign_id
        
        campaign_entity( const Alloc & a ) :
            base_entity<Alloc>(a),
            campaign_id{}
        {}
            
        uint32_t campaign_id;
        
        template<typename Key, typename Serializable>
        void store(Key && key, Serializable  && data)  {
            base_type::store(std::forward<Serializable>(data)) ;
            campaign_id = key.template get<campaign_id_tag>();  
        }
        template<typename Serializable>
        static std::size_t size(const Serializable && data) {
            std::stringstream ss;
            boost::archive::binary_oarchive oarch(ss);
            oarch << std::forward<Serializable>(data) ;
            return base_type::size(std::forward<Serializable>(data)) +                
                   sizeof(data.campaign_id) +                
                   ss.str().size() ;
        }
        template<typename Serializable>
        void retrieve(Serializable  & data) const {
            base_type::template retrieve(data) ;
        }
        //needed for ability to update after matching by calling index.modify(itr,entry)
        void operator()(campaign_entity &entry) const {
            base_entity<Alloc>::operator()(static_cast<base_type &>(entry));            
            entry.campaign_id=campaign_id;
        }
    };
   
 
 
template<typename Alloc>
using campaign_container =
boost::multi_index_container<
    campaign_entity<Alloc>,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<typename campaign_entity<Alloc>::campaign_id_tag>,
              BOOST_MULTI_INDEX_MEMBER(campaign_entity<Alloc>,uint32_t,campaign_id)
        >
    >,
    boost::interprocess::allocator<campaign_entity<Alloc>,typename Alloc::segment_manager>
> ;
  
  
}}

#endif /* __IPC_DATA_CAMPAIGN_ENTITY_HPP__ */

